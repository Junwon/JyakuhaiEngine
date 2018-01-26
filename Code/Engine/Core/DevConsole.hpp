#pragma once

#include <vector>
#include <map>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vector2.hpp"

class Rgba;
class SimpleRenderer;
class Font;
class DevConsole;

const Rgba DEV_BACKGROUND_COLOR = Rgba(0, 0, 0, 100);
const Rgba TEXT_COLOR = Rgba(35, 225, 67);
const Rgba UI_COLOR = Rgba(82, 135, 233, 232);
const float UI_LINE_HEIGHT = 30.f;

const Vector2 ONSET_POS = Vector2(8.f, 35.f);

const float DEFAULT_FONT_SIZE = 1.f;
const float FONT_SIZE_DIVISOR = 0.1f;
const float TEXT_X_OFFSET = 20.f;
const float TEXT_Y_OFFSET = 0.f;

const Vector2 CURRENT_TEXT_LINE_POS = Vector2(ONSET_POS.x + TEXT_X_OFFSET, ONSET_POS.y + TEXT_Y_OFFSET);

const float CURSOR_MAX_BLINK_DUR = 0.4f;
const float CURSOR_HEIGHT = 20.f;
const float CURSOR_X_OFFSET = 0.0f;
const Rgba CURSOR_COLOR = TEXT_COLOR;

struct ConsoleText
{
	std::string text;
	Rgba color;
};

class ConsoleArgs
{
public:
	ConsoleArgs(DevConsole* console, const std::string& command, const std::string& arguments);

public:
	std::string GetArgumentsAsString() const;
	size_t GetNumArguments() const;

public:
	DevConsole* m_devConsole;
	std::string m_command;
	std::vector<std::string> m_arguments;

private:
	unsigned int m_localCursorPos;
};

typedef void(*ConsoleFunction)(ConsoleArgs& args);

struct ConsoleCommand
{
	ConsoleCommand() {};
	ConsoleCommand(const std::string& desc, ConsoleFunction functionPtr, const std::string& tag = "") :
		m_description(desc),
		m_functionPtr(functionPtr),
		m_tag(tag)
	{}

	std::string m_description;
	ConsoleFunction m_functionPtr;
	std::string m_tag;
};

//////////////////////////////////////////////////////////////////////////

class DevConsole
{
public:
	DevConsole(SimpleRenderer* renderer);
	~DevConsole();

	void Update(float deltaSeconds);
	void Render() const;
	void Enable();
	void Disable();
	bool Toggle();
	bool IsEnabled() const;
	void RegisterKeyDown(unsigned char keyCode);
	void RegisterKeyUp(unsigned char keyCode);
	void RegisterInputs(unsigned char character);
	void ProcessConsoleLine();
	void ProcessConsoleCommand(const std::string& commandAndArgs);
	ConsoleArgs BuildConsoleArgs(const std::string& inputBuffer);
	const ConsoleCommand* FindCommand(const std::string& command);
	void ConsolePrintf(const Rgba& color, char const *format, ...);
	void SetFont(Font* setFont);
	void SetFontSize(float size);
	void RegisterConsoleCommand(const std::string& commandName, const std::string& desc, ConsoleFunction funcPtr, const std::string& tag = "");
	void PrintConsoleCommandsHelpText();

	void ClearHistory();
public:
	unsigned int m_width;
	unsigned int m_height;
	bool m_enabled;

private:
	void RegisterDevCommands();
	void DecrementCursorIndexPos();
	void IncrementCursorIndexPos();
	void UpdateCursor(float deltaSeconds);
	void RenderConsole() const;

	void RenderConsoleProcessedTexts() const;

	void RenderConsoleCurrentLine() const;

	void RenderConsoleGraphics() const;

	void RenderConsoleBase() const;
	void RenderCursor() const;
	void InsertInputIntoCurrentInputLine(unsigned char input);
	void ClearCurrentInputLine();
	void SetLaterHistoryLinesToCurrent();
	void SetEarlierHistoryLinesToCurrent();
	bool SetHistoryLinesToCurrentAtIndex(int index);
	void InsertLineIntoProcessedTexts(const ConsoleText& ct);
public:
	SimpleRenderer* m_devRenderer;

	float m_fontSize;

	std::string m_currentInputLine;
	std::string m_lastProcessedLine;
	float m_heightSpacing;

	float m_cursorBlinkDuration;
	bool m_isCursorVisible;

	bool m_justTurnedOn;

	unsigned int m_cursorIndexPos;

	Rgba m_textColor;

	std::vector<ConsoleText> m_processedTexts;

	std::vector<std::string> m_commandHistory;
	int m_historyIndex;

	std::map<std::string, ConsoleCommand> m_commands;
};