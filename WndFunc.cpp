#include "WndFunc.h"
#include "menuSupport.h"
#include "Display.h"
#include "Model.h"

#include <unordered_map>
#include <string>
#include <string_view>
#include <optional>
#include <charconv>

enum class PARSE : uint8_t
{
	WINDOW_WIDTH = 0,
	WINDOW_HEIGHT,
	PIXEL_SIZE,
	MEMORY_SIZE
};

[[nodiscard]] size_t removeSpaces(std::string_view sv, size_t pos)
{
	while (pos < sv.size() && sv[pos] == ' ') {
		pos++;
	}
	return pos == sv.size() ? sv.npos : pos;
}

[[nodiscard]] std::optional<int> takeInteger(const char* sv, size_t b, size_t e)
{
	if (int value; std::from_chars(sv + b, sv + e, value).ec == std::errc{}) {
		return value;
	}
	else {
		return std::nullopt;
	}
};

[[nodiscard]] std::unordered_map<PARSE, int> parseData(const char* bufferData)
{
	if (!bufferData) {
		return {};
	}

	std::unordered_map<PARSE, int> result;
	std::string_view sv(bufferData);
	std::unordered_map<std::string, PARSE> token_parse = { 
		{"WINDOW_WIDTH", PARSE::WINDOW_WIDTH},
		{"WINDOW_HEIGHT",PARSE::WINDOW_HEIGHT},
		{"PIXEL_SIZE", PARSE::PIXEL_SIZE},
		{"MEMORY_SIZE", PARSE::MEMORY_SIZE} 
	};

	for (const auto& [token, parse] : token_parse) {
		size_t pos = sv.find(token.c_str());

		// We are did not find neccessary token
		if (pos == sv.npos) {
			return {};
		}

		pos += token.size();
		pos = removeSpaces(sv, pos);

		if (pos == sv.npos || sv[pos] != '=') {
			return {};
		}

		pos++;
		pos = removeSpaces(sv, pos);
		size_t pos_e = sv.find(';', pos);

		auto num = takeInteger(bufferData, pos, pos_e);

		if (!num.has_value()) {
			return {};
		}

		result[parse] = num.value();
	}

	return result;
}

void WndCreate(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	using namespace jd;

	Display* image = nullptr;
	Window* window = nullptr;

	try {
		window = new Window{};
	} 
	catch (const std::bad_alloc& e) {
		MessageBoxA(hwnd, (LPCSTR)(e.what()), (LPCSTR)L"Error Window Allocation", MB_OK | MB_ICONERROR);
		PostMessage(hwnd, WM_DESTROY, 0, 0);
		return;
	}

	HANDLE ConfigOnLoad = CreateFileA(
		"config.txt",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (ConfigOnLoad != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER size;
		GetFileSizeEx(ConfigOnLoad, &size);
		char* bufferData = new char[size.QuadPart];

		DWORD bytesIterated;
		if (ReadFile(ConfigOnLoad, bufferData, size.QuadPart, &bytesIterated, NULL))
		{
			auto parse_map_data = parseData(bufferData);
			if (!parse_map_data.empty())
			{
				auto& m_config = window->getWindowConfig();
				m_config.d_WIDTH = parse_map_data[PARSE::WINDOW_WIDTH];
				m_config.d_HEIGHT = parse_map_data[PARSE::WINDOW_HEIGHT];
				m_config.d_PIXEL_SIZE = parse_map_data[PARSE::PIXEL_SIZE];
				m_config.d_MEMORY_SIZE = parse_map_data[PARSE::MEMORY_SIZE];
			}

			window->setAllocatorSize();
		}

		CloseHandle(ConfigOnLoad);
	}

	try {
		// we do not apply checks for the maximum and current size
		const auto const& m_config = window->getWindowConfig();
		image = new Display { 
			m_config.d_WIDTH,
			m_config.d_HEIGHT,
			m_config.d_DEPTH,
			m_config.d_PIXEL_SIZE,
			new jdByte[m_config.d_MAX_WIDTH * m_config.d_MAX_HEIGHT * 4],
			new jdByte[m_config.d_MAX_WIDTH * m_config.d_MAX_HEIGHT]
		};
	}
	catch (const std::bad_alloc& e) {
		MessageBoxA(hwnd, (LPCSTR)(e.what()), (LPCSTR)L"Error Image Allocation", MB_OK | MB_ICONERROR);
		PostMessage(hwnd, WM_DESTROY, 0, 0);
		return;
	}

	WndAddMenus(hwnd);

	SetWindowLongPtr(hwnd, 0, (LONG_PTR)image);
	SetWindowLongPtr(hwnd, sizeof(Display*), (LONG_PTR)window);
}