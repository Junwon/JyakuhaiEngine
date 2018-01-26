#include "Engine/Core/DevConsole.hpp"

#include "Engine/Renderer/RHI/SimpleRenderer.hpp"

#include "Engine/Core/EngineBase.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Font.hpp"

const int MESSAGE_MAX_LENGTH = 2048;

//////////////////////////////////////////////////////////////////////////

void Help(ConsoleArgs& args)
{
	args.m_devConsole->PrintConsoleCommandsHelpText();
}

void Clear(ConsoleArgs& args)
{
	args.m_devConsole->ClearHistory();
}

void Exit(ConsoleArgs& args)
{
	args.m_devConsole->Disable();
}

void RunSetFontSize(ConsoleArgs& args)
{
	args.m_devConsole->SetFontSize((float)stof(args.m_arguments[0]));
}

void RunSpawnConsole(ConsoleArgs& args)
{
	int numClients = 1;
	if (!args.m_arguments.empty()) {
		numClients = stoi(args.m_arguments[0]);
	}

	for (int i = 0; i < numClients; ++i) {
		char buffer[256];
		::GetModuleFileNameA(nullptr, &buffer[0], 256);

		STARTUPINFOA startupInfo = { sizeof(STARTUPINFO) };
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW;
		startupInfo.wShowWindow = SW_SHOW;
		PROCESS_INFORMATION pi;

		::CreateProcessA(&buffer[0], nullptr, nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &pi);
	}
}

//class Rawr;
void RunDLLFunction(ConsoleArgs&) {
	//ListDLLFunctions("TMXDummy.dll");

// 	HMODULE libHandle = LoadLibraryA("TMXDummy.dll");
// 	void* func = GetProcAddress(libHandle, "NewRawr");
// 
// 	if (func) {
// 		DebuggerPrintlnf("yay");
// 		Rawr* (*_cdecl NewRawr) ();
// 		NewRawr = (Rawr* (*_cdecl)())func;
// 	}
// 	else {
// 		DebuggerPrintlnf("nope");
// 	}
}

//////////////////////////////////////////////////////////////////////////

DevConsole::DevConsole(SimpleRenderer* renderer) :
	m_width(renderer->m_width),
	m_height(renderer->m_height),
	m_fontSize(DEFAULT_FONT_SIZE),
	m_devRenderer(renderer),
	m_currentInputLine(""),
	m_lastProcessedLine(""),
	m_historyIndex(-1),
	m_enabled(false),
	m_cursorBlinkDuration(CURSOR_MAX_BLINK_DUR),
	m_isCursorVisible(true),
	m_cursorIndexPos(0),
	m_justTurnedOn(false),
	m_textColor(Rgba::WHITE),
	m_heightSpacing((float)m_devRenderer->mainFont->lineHeight * DEFAULT_FONT_SIZE)
{
	RegisterDevCommands();
}

DevConsole::~DevConsole()
{
}

void DevConsole::RegisterDevCommands()
{
	RegisterConsoleCommand("help", "Displays helpful description of available console commands.", Help);
	RegisterConsoleCommand("exit", "Exits from the console.", Exit);
	RegisterConsoleCommand("clear", "Clears the console.", Clear);
	RegisterConsoleCommand("set_font_size", "param: <size> Sets the font size.", RunSetFontSize);
	RegisterConsoleCommand("spawn_console", "Spawns a new console", RunSpawnConsole);
	//RegisterConsoleCommand("black_magic", "spooky things", RunDLLFunction);
}

void DevConsole::Update(float deltaSeconds)
{
	if (m_enabled)
	{
		m_textColor = Rgba::WHITE;

		UpdateCursor(deltaSeconds);
	}
}

void DevConsole::UpdateCursor(float deltaSeconds)
{
	m_cursorBlinkDuration -= deltaSeconds;

	if (m_cursorBlinkDuration <= 0)
	{
		m_isCursorVisible = !m_isCursorVisible;
		m_cursorBlinkDuration = CURSOR_MAX_BLINK_DUR;
	}
}

void DevConsole::Render() const
{
	PROFILE_SCOPE_FUNCTION();
	if (m_enabled)
	{
		RenderConsole();
		RenderCursor();
	}
}

void DevConsole::RenderConsole() const
{
	RenderConsoleBase();

	RenderConsoleGraphics();

	RenderConsoleCurrentLine();

	RenderConsoleProcessedTexts();
}

void DevConsole::RenderConsoleBase() const
{
	m_devRenderer->SetUIOrthoProjection(Vector2(0.f, 0.f), Vector2((float)m_width, (float)m_height));
	m_devRenderer->SetShader(nullptr);
	m_devRenderer->DrawQuad2D(0.f, 0.f, (float)m_width, (float)m_height, DEV_BACKGROUND_COLOR);
}

void DevConsole::RenderConsoleGraphics() const
{
	m_devRenderer->DrawLine2D(Vector2(0.f, UI_LINE_HEIGHT), Vector2((float)m_devRenderer->m_width, UI_LINE_HEIGHT), UI_COLOR);

	m_devRenderer->DrawText2D(ONSET_POS, ">", m_fontSize, UI_COLOR);
}

void DevConsole::RenderConsoleCurrentLine() const
{
	m_devRenderer->DrawText2D(CURRENT_TEXT_LINE_POS, m_currentInputLine, m_fontSize, TEXT_COLOR);
}

void DevConsole::RenderConsoleProcessedTexts() const
{
	for (int index = (int)(m_processedTexts.size() - 1); index >= 0; --index)
	{
		ConsoleText ct = m_processedTexts[index];
		m_devRenderer->DrawText2D(CURRENT_TEXT_LINE_POS.x, (CURRENT_TEXT_LINE_POS.y + ((m_processedTexts.size() - index) * m_heightSpacing)), ct.text, m_fontSize, ct.color);
	}
}

void DevConsole::RenderCursor() const
{
	if (m_isCursorVisible)
	{
		std::string cursorSubstring = m_currentInputLine.substr(0, m_cursorIndexPos);
		float cursorSubstringWidth = (float)(m_devRenderer->mainFont->CalculateTextWidth(cursorSubstring)) * m_fontSize;
		float cursorXPos = CURRENT_TEXT_LINE_POS.x + cursorSubstringWidth + CURSOR_X_OFFSET;

		m_devRenderer->DrawLine2D(Vector2(cursorXPos, CURRENT_TEXT_LINE_POS.y),
			Vector2(cursorXPos, CURRENT_TEXT_LINE_POS.y + CURSOR_HEIGHT), 
			CURSOR_COLOR);
	}
	
}

void DevConsole::InsertInputIntoCurrentInputLine(unsigned char input)
{
	m_currentInputLine.insert(m_currentInputLine.begin() + m_cursorIndexPos, input);
	IncrementCursorIndexPos();
}

void DevConsole::ClearCurrentInputLine()
{
	m_currentInputLine.clear();
	m_cursorIndexPos = 0;
}

void DevConsole::SetLaterHistoryLinesToCurrent()
{
	if (SetHistoryLinesToCurrentAtIndex(m_historyIndex)) {
		m_historyIndex--;
		if (m_historyIndex < 0) {
			m_historyIndex = 0;
		}
	}
}

void DevConsole::SetEarlierHistoryLinesToCurrent()
{
	if (SetHistoryLinesToCurrentAtIndex(m_historyIndex + 1)) {
		m_historyIndex++;
	}
	else {
		ClearCurrentInputLine();
	}
}

bool DevConsole::SetHistoryLinesToCurrentAtIndex(int index)
{
	if (index < 0 || index >= (int)m_commandHistory.size()) {
		return false;
	}
	else {
		m_currentInputLine = m_commandHistory[index];
		m_cursorIndexPos = m_currentInputLine.size();
		return true;
	}
}

void DevConsole::InsertLineIntoProcessedTexts(const ConsoleText& ct)
{
	m_processedTexts.push_back(ct);
}

void DevConsole::PrintConsoleCommandsHelpText()
{
	for (const auto cm : m_commands)
	{
		// Print commands with no tags
		if (cm.second.m_tag.empty()) {
			ConsolePrintf(TEXT_COLOR, "%s - %s",cm.first.data(), cm.second.m_description.data());
		}
	}
}

void DevConsole::ClearHistory()
{
	m_processedTexts.clear();
}

void DevConsole::Enable()
{
	m_enabled = true;
}

void DevConsole::Disable()
{
	m_enabled = false;
}

bool DevConsole::Toggle()
{
	m_enabled = !m_enabled;

	if (m_enabled)
	{
		DebuggerPrintlnf("Console Opened");
	}
	else
	{
		DebuggerPrintlnf("Console Closed");
	}

	return m_enabled;
}

bool DevConsole::IsEnabled() const
{
	return m_enabled;
}

void DevConsole::RegisterKeyDown(unsigned char keyCode)
{
	if (keyCode == KEYCODE_TILDE)
	{
		if (!m_justTurnedOn && !m_enabled)
		{
			m_justTurnedOn = true;
		}
	}
	else if (keyCode == KEYCODE_RIGHT)
	{
		IncrementCursorIndexPos();
	}
	else if (keyCode == KEYCODE_LEFT)
	{
		DecrementCursorIndexPos();
	}
}

void DevConsole::RegisterKeyUp(unsigned char keyCode)
{
	if (m_enabled && keyCode == KEYCODE_ESCAPE )
	{
		Toggle();
	}

	if (keyCode == KEYCODE_TILDE)
	{
		if (m_justTurnedOn && m_enabled)
		{
			m_justTurnedOn = false;
		}
		else if(!m_justTurnedOn && m_enabled)
		{
			Toggle();
		}
	}

	if (keyCode == KEYCODE_UP) {
		SetLaterHistoryLinesToCurrent();
	}
	else if (keyCode == KEYCODE_DOWN) {
		SetEarlierHistoryLinesToCurrent();
	}
}

void DevConsole::RegisterInputs(unsigned char keyInput)
{
	if (IsEnabled())
	{
		if (keyInput == KEYCODE_BACKSPACE)
		{
			DecrementCursorIndexPos();
			m_currentInputLine.erase(m_currentInputLine.begin() + m_cursorIndexPos);
		}
		else if (keyInput == KEYCODE_ESCAPE || keyInput == '`')
		{
			return;
		}
		else if (keyInput == KEYCODE_SPACE)
		{
			InsertInputIntoCurrentInputLine(' ');
			m_cursorBlinkDuration = CURSOR_MAX_BLINK_DUR;
		}
		else if (keyInput == KEYCODE_ENTER && m_currentInputLine.size() > 0)
		{
			if (m_currentInputLine == "qq")
			{
				ConsolePrintf(Rgba::BLUE, "Q_____Q");
			}
			else if (IsStringNotOnlyWhiteSpaces(m_currentInputLine))
			{
				ProcessConsoleLine();
			}

			ClearCurrentInputLine();
		}
		else if (keyInput != KEYCODE_ENTER)
		{
			InsertInputIntoCurrentInputLine(keyInput);
		}
	}
}

void DevConsole::DecrementCursorIndexPos()
{
	if (!m_currentInputLine.empty() && m_cursorIndexPos > 0)
	{
		m_cursorIndexPos--;
	}
}

void DevConsole::IncrementCursorIndexPos()
{
	if (!m_currentInputLine.empty() && m_cursorIndexPos < m_currentInputLine.size())
	{
		m_cursorIndexPos++;
	}
}

void DevConsole::ProcessConsoleLine()
{
	ProcessConsoleCommand(m_currentInputLine);

	m_commandHistory.push_back(m_currentInputLine);
	m_historyIndex = m_commandHistory.size() - 1;
//	m_lastProcessedLine = m_currentInputLine;
}

void DevConsole::ProcessConsoleCommand(const std::string& commandAndArgs)
{
	ConsoleArgs consoleArgs = BuildConsoleArgs(commandAndArgs);
	const ConsoleCommand* command = FindCommand(consoleArgs.m_command);

	if (command)
	{
		command->m_functionPtr(consoleArgs);
	}
	else
	{
		ConsolePrintf(Rgba::RED, "Error: \'%s\' Is an invalid command.", commandAndArgs.c_str());
	}
}

ConsoleArgs DevConsole::BuildConsoleArgs(const std::string& inputBuffer)
{
	std::string commandName;
	std::string args;

	unsigned int firstWhitespaceChar = 0;
	for (unsigned int charIndex = 0; charIndex < inputBuffer.size(); ++charIndex) 
	{
		if (inputBuffer[charIndex] == ' ') {
			firstWhitespaceChar = charIndex;
			break;
		}

		firstWhitespaceChar = charIndex;
		commandName += inputBuffer[charIndex];
	}

	args = inputBuffer.substr(firstWhitespaceChar + 1);

	return ConsoleArgs(this, commandName, args);
}

const ConsoleCommand* DevConsole::FindCommand(const std::string& command)
{
	std::map<std::string, ConsoleCommand>::const_iterator it = m_commands.begin();

	for (; it != m_commands.end(); ++it) 
	{
		if ((*it).first == command) {
			return &(*it).second;
		}
	}

	return nullptr;
}

void DevConsole::ConsolePrintf(const Rgba& color, const char* format, ...)
{
	char messageLiteral[MESSAGE_MAX_LENGTH];
	va_list variableArgumentList;
	va_start(variableArgumentList, format);
	vsnprintf_s(messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, format, variableArgumentList);
	va_end(variableArgumentList);
	messageLiteral[MESSAGE_MAX_LENGTH - 1] = '\0'; 

	ConsoleText ct;
	ct.color = color;
	ct.text = messageLiteral;
	InsertLineIntoProcessedTexts(ct);
}

void DevConsole::SetFont(Font* setFont)
{
	m_devRenderer->mainFont = setFont;
}

void DevConsole::SetFontSize(float size)
{
	if (size > 0.f) {
		if (size < 1.f) {
			m_fontSize = size;
		}
		else {
			m_fontSize = 1 + size * FONT_SIZE_DIVISOR;
		}
	}
}

void DevConsole::RegisterConsoleCommand(const std::string& commandName, const std::string& desc, ConsoleFunction funcPtr, const std::string& tag)
{
	m_commands[commandName] = ConsoleCommand(desc, funcPtr, tag);
}

//////////////////////////////////////////////////////////////////////////

ConsoleArgs::ConsoleArgs(DevConsole* console, const std::string& command, const std::string& arguments) :
	m_devConsole(console),
	m_command(command),
	m_localCursorPos(0)
{
	SplitIntoBuffer(m_arguments, arguments);
}

std::string ConsoleArgs::GetArgumentsAsString() const
{
	std::string result = "";

	if (!m_arguments.empty()) {

		result = m_arguments[0];

		for (int i = 1; i < (int)m_arguments.size(); ++i) {
			result += " " + m_arguments[i];
		}
	}

	return result;
}

size_t ConsoleArgs::GetNumArguments() const
{
	return m_arguments.size();
}
