#include "Engine/Core/Performance/ProfilerReport.hpp"

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RHI/SimpleRenderer.hpp"
#include "Engine/Math/AABB2D.hpp"

#include "Engine/Core/Performance/ProfilerSystem.hpp"
#include "Engine/Core/Performance/PerformanceCommon.hpp"

float WindowWidth = 0.f;
float WindowHeight = 0.f;

float VisualizerMinX = 0.f;
float VisualizerMinY = (float)(WindowHeight - 470.f);
float VisualizerMaxX = (float)(WindowHeight);
float VisualizerMaxY = (float)(WindowHeight - 250.f);
float VisualizerGraphMinX = (float)(WindowHeight * 0.3f);
float VisualizerGraphMinY = VisualizerMinY * 1.11f;
float VisualizerGraphMaxX = (float)(WindowHeight * 0.85f);
float VisualizerGraphMaxY = VisualizerMaxY * .95f;
float MemoryDataWidth = (VisualizerGraphMaxX - VisualizerGraphMinX) / 200.f;
float MemoryDataHeight = (VisualizerGraphMaxY - VisualizerGraphMinY);
Vector2 CPUTextPos = Vector2(5.f, (float)(WindowHeight - 300.f));
Vector2 LastFrameTimeTextPos = Vector2(15.f, (float)(WindowHeight - 330.f));
Vector2 FrameViewTextPos = Vector2(5.f, (float)(WindowHeight - 420.f));
Vector2 ReportHeaderTextPos = Vector2(5.f, (float)(WindowHeight - 460.f));
Vector2 ReportTextPos = Vector2(5.f, (float)(WindowHeight - 490.f));

float ReportTextYPos = (float)(WindowHeight - 490.f);

const float ReportFunctionNameTextXPos = 5.f;
const float ReportCallsTextXPos = 775.f;
const float ReportTotalPercentageTimeTextXPos = 950.f;
const float ReportTotalTimeTextXPos = 1100.f;
const float ReportSelfPercentageTimeTextXPos = 1250.f;
const float ReportSelfTimeTextXPos = 1400.f;

const float YTextSpacing = -30.f;
const float ReportIndentationAmount = 30.f;

const char* REPORT_HEADER_FORMAT = "%-110s%-20s%-20s%-20s%-20s%-20s";
const char* REPORT_LOG_FORMAT = "%-110s%-20d%-20s%-20s%-20s%-20s";
const char* REPORT_TAG = "profiler";

const Rgba GRAPH_COLOR = Rgba(255, 255, 250, 180);

const double HIGH_THRESHOLD = 60;
const double MID_HIGH_THRESHOLD = 40;
const double MID_LOW_THRESHOLD = 30;
const double LOW_THRESHOLD = 15;

SimpleRenderer* m_profilerRenderer = nullptr;

//------------------------------------------------------------------------
//------------------------------------------------------------------------

ProfilerReport::ProfilerReport() :
	m_latestFrame(nullptr),
	m_needsUpdate(false),
	m_view(REPORT_TREE_VIEW),
	m_flatViewDefaultSort(SORT_SELF_TIME)
{
	m_frametimeHistory.reserve(MAX_MEMORY_USAGE_HISTORY_SIZE);
}

ProfilerReport::ProfilerReport(ProfilerNode_T* frame) :
	m_latestFrame(frame),
	m_needsUpdate(false),
	m_view(REPORT_TREE_VIEW),
	m_flatViewDefaultSort(SORT_SELF_TIME)
{
	m_frametimeHistory.reserve(MAX_MEMORY_USAGE_HISTORY_SIZE);
}

void ProfilerReport::SetLatestFrame(ProfilerNode_T * frame)
{
	m_latestFrame = frame;
}

//------------------------------------------------------------------------
void ProfilerReport::ProfilerUpdate(float deltaSeconds)
{
	if (m_latestFrame && m_needsUpdate) {
		CleanUpOldReports();
		ParseInformation(m_lastFramesReport, m_latestFrame);
		TraverseAndAddReport(m_latestFrame);
		AddFrameTimeToHistory(deltaSeconds);
	}
	m_needsUpdate = false;
}

//------------------------------------------------------------------------
void ProfilerReport::CleanUpOldReports()
{
	for (int i = 0; i < (int)m_treeReports.size(); ++i) {
		SAFE_DELETE(m_treeReports[i]);
	}

	m_treeReports.clear();
	m_flatReports.clear();
}

//------------------------------------------------------------------------
void ProfilerReport::AddFrameTimeToHistory(float deltaSeconds)
{
	if (m_frametimeHistory.size() >= MAX_MEMORY_USAGE_HISTORY_SIZE) {
		m_frametimeHistory.erase(m_frametimeHistory.begin());
	}

	m_frametimeHistory.push_back(1.f / deltaSeconds);
}

//------------------------------------------------------------------------
void ProfilerReport::ProfilerSetRenderer(SimpleRenderer* renderer)
{
	m_profilerRenderer = renderer;
	WindowWidth = (float)m_profilerRenderer->m_width;
	WindowHeight = (float)m_profilerRenderer->m_height;

	UpdateGraphicsMembers();
}

//------------------------------------------------------------------------
void ProfilerReport::UpdateGraphicsMembers()
{
	VisualizerMinX = 0;
	VisualizerMinY = (float)(WindowHeight - 470.f);
	VisualizerMaxX = (float)(WindowWidth);
	VisualizerMaxY = (float)(WindowHeight - 250.f);
	VisualizerGraphMinX = (float)(WindowWidth * 0.3f);
	VisualizerGraphMinY = VisualizerMinY * 1.11f;
	VisualizerGraphMaxX = (float)(WindowWidth * 0.85f);
	VisualizerGraphMaxY = VisualizerMaxY * .95f;
	MemoryDataWidth = (VisualizerGraphMaxX - VisualizerGraphMinX) / 200.f;
	MemoryDataHeight = (VisualizerGraphMaxY - VisualizerGraphMinY);
	CPUTextPos = Vector2(5.f, (float)(WindowHeight - 300.f));
	LastFrameTimeTextPos = Vector2(15.f, (float)(WindowHeight - 330.f));
	FrameViewTextPos = Vector2(5.f, (float)(WindowHeight - 420.f));
	ReportHeaderTextPos = Vector2(5.f, (float)(WindowHeight - 460.f));
	ReportTextPos = Vector2(5.f, (float)(WindowHeight - 490.f));

	ReportTextYPos = (float)(WindowHeight - 490.f);
}

//------------------------------------------------------------------------
void ProfilerReport::ProfilerRender() const
{
	PROFILE_SCOPE_FUNCTION();
	ASSERT_OR_DIE(m_profilerRenderer, "Profiler Renderer is not set.");

	AABB2D bound = AABB2D(1.f, 1.f, m_profilerRenderer->GetWindow()->m_width - 16.f, m_profilerRenderer->GetWindow()->m_height * 0.65f);

	m_profilerRenderer->DrawBorderedQuad2D(bound, AABB2D::ZERO_TO_ZERO, 1.f, Rgba::TRANSPARENT_BLACK);

	DrawTextsForProfilerRendering();
	DrawGraphForProfilerRendering();
}

//------------------------------------------------------------------------
void ProfilerReport::SortedInsertIntoFlatView(ProfilerReportNode_T* report)
{
	if (m_flatReports.empty()) {
		m_flatReports.push_back(report);
		return;
	}

	int insertIndex = -1;

	for (int i = 0; i < (int)m_flatReports.size(); ++i)
	{
		ProfilerReportNode_T* currentReport = m_flatReports[i];

		double insertingValue = 0.f;
		double comparedValue = 0.f;

		switch (m_flatViewDefaultSort) {
		case SORT_TOTAL_PERCENT:
			insertingValue = report->m_totalPercentage;
			comparedValue = currentReport->m_totalPercentage;
			break;
		case SORT_TOTAL_TIME:
			insertingValue = report->m_totalTime;
			comparedValue = currentReport->m_totalTime;
			break;
		case SORT_SELF_PERCENT:
			insertingValue = report->m_selfPercent;
			comparedValue = currentReport->m_selfPercent;
			break;
		case SORT_SELF_TIME:
			insertingValue = report->m_selfTime;
			comparedValue = currentReport->m_selfTime;
			break;
		}
		
		if (comparedValue < insertingValue) {
			insertIndex = i;
			break;
		}
	}

	if (insertIndex < 0) {
		m_flatReports.push_back(report);
	}
	else {
		m_flatReports.insert(m_flatReports.begin() + insertIndex, report);
	}
}

//------------------------------------------------------------------------
void ProfilerReport::DrawTextsForProfilerRendering() const
{
	m_profilerRenderer->DrawText2D(CPUTextPos, "CPU", 1.f);
	std::string lastFrameTimeText = GetProperTimeSecondsString(ProfilerGetLastFrameTimeSeconds());
	m_profilerRenderer->DrawText2D(LastFrameTimeTextPos, Stringf("Last Frame Time: %s", lastFrameTimeText.c_str()));

	if (m_view == REPORT_TREE_VIEW) {
		m_profilerRenderer->DrawText2D(FrameViewTextPos, "FRAME TREE VIEW");
	}
	else if (m_view == REPORT_FLAT_VIEW) {
		m_profilerRenderer->DrawText2D(FrameViewTextPos, "FRAME FLAT VIEW");
	}

	m_profilerRenderer->DrawText2D(ReportHeaderTextPos, Stringf(REPORT_HEADER_FORMAT,
		"FUNCTION NAME",
		"CALLS",
		"% TOTAL",
		"(TIME)",
		"% SELF",
		"(TIME)"));

	if (m_view == REPORT_TREE_VIEW) {
		RenderTreeViewText();
	}
	else if (m_view == REPORT_FLAT_VIEW) {
		RenderFlatViewText();
	}
}

//------------------------------------------------------------------------
void ProfilerReport::RenderTreeViewText() const
{
	Vector2 currentReportTextPos = ReportTextPos;
	float currentYPos = ReportTextYPos;
	for (ProfilerReportNode_T* report : m_treeReports) {
		m_profilerRenderer->DrawText2D(Vector2(ReportFunctionNameTextXPos + (report->m_treeHeight * ReportIndentationAmount), currentYPos), report->m_tag.c_str());

		DrawProfilerData(report, currentYPos);
		currentYPos += YTextSpacing;
	}
}

//------------------------------------------------------------------------
void ProfilerReport::RenderFlatViewText() const
{
	Vector2 currentReportTextPos = ReportTextPos;
	float currentYPos = ReportTextYPos;
	for (ProfilerReportNode_T* report : m_flatReports) {
		m_profilerRenderer->DrawText2D(Vector2(ReportFunctionNameTextXPos, currentYPos), report->m_tag.c_str());

		DrawProfilerData(report, currentYPos);
		currentYPos += YTextSpacing;
	}
}

//------------------------------------------------------------------------
void ProfilerReport::DrawProfilerData(ProfilerReportNode_T* report, float textYPos) const
{
	std::string totalPercentage = Stringf("%3.2f%%", report->m_totalPercentage);
	std::string totalTime = GetProperTimeSecondsString(report->m_totalTime);
	std::string selfPercentage = Stringf("%3.2f%%", report->m_selfPercent);
	std::string selfTime = GetProperTimeSecondsString(report->m_selfTime);

	m_profilerRenderer->DrawText2D(Vector2(ReportCallsTextXPos, textYPos), Stringf("%d", report->m_calls));
	m_profilerRenderer->DrawText2D(Vector2(ReportTotalPercentageTimeTextXPos, textYPos), totalPercentage.c_str());
	m_profilerRenderer->DrawText2D(Vector2(ReportTotalTimeTextXPos, textYPos), totalTime.c_str());
	m_profilerRenderer->DrawText2D(Vector2(ReportSelfPercentageTimeTextXPos, textYPos), selfPercentage.c_str());
	m_profilerRenderer->DrawText2D(Vector2(ReportSelfTimeTextXPos, textYPos), selfTime.c_str());
}

//------------------------------------------------------------------------
void ProfilerReport::DrawGraphForProfilerRendering() const
{
	m_profilerRenderer->DrawQuad2D(VisualizerGraphMinX, VisualizerGraphMinY, VisualizerGraphMaxX, VisualizerGraphMaxY, GRAPH_COLOR);

	for (int i = 0; i < (int)m_frametimeHistory.size(); ++i) {
		double currentFrametime = m_frametimeHistory[i];

		float bottomX = VisualizerGraphMinX + (MemoryDataWidth * i);
		float bottomY = VisualizerGraphMinY;
		float topX = VisualizerGraphMinX + (MemoryDataWidth * (i + 1));
		float topY = VisualizerGraphMinY + (MemoryDataHeight);

		Rgba dataColor = Rgba::RED;

		if (currentFrametime >= HIGH_THRESHOLD) {
			dataColor = Rgba::GREEN;
		}
		else if (currentFrametime < HIGH_THRESHOLD && currentFrametime >= MID_HIGH_THRESHOLD) {
			dataColor = Rgba(190, 215, 10);
		}
		else if (currentFrametime < MID_HIGH_THRESHOLD && currentFrametime >= MID_LOW_THRESHOLD) {
			dataColor = Rgba::YELLOW;
		}
		else if (currentFrametime < MID_LOW_THRESHOLD && currentFrametime >= LOW_THRESHOLD) {
			dataColor = Rgba::ORANGE;
		}

		m_profilerRenderer->DrawQuad2D(bottomX, bottomY, topX, topY, dataColor);
	}
}

//------------------------------------------------------------------------
void ProfilerReport::Log()
{
	LogTaggedPrintf(REPORT_TAG, REPORT_HEADER_FORMAT, 
		"TAG NAME",
		"CALLS",
		"TOTAL %",
		"TOTAL TIME",
		"SELF %",
		"SELF TIME");

	if (m_view == REPORT_TREE_VIEW) {
		TraverseAndLogThroughTree(m_latestFrame);
	}
	else if (m_view == REPORT_FLAT_VIEW) {
		if (!m_flatReports.empty()) {
			LogFlatReports(m_flatReports);
		}
	}
}

//------------------------------------------------------------------------
void ProfilerReport::LogFlatReports(std::vector<ProfilerReportNode_T *> reports)
{
	for (ProfilerReportNode_T* report : reports) {
		std::string totalPercentage = Stringf("%3.2f%%", report->m_totalPercentage);
		std::string totalTime = GetProperTimeSecondsString(report->m_totalTime);
		std::string selfPercentage = Stringf("%3.2f%%", report->m_selfPercent);
		std::string selfTime = GetProperTimeSecondsString(report->m_selfTime);

		LogTaggedPrintf(REPORT_TAG, REPORT_LOG_FORMAT, report->m_tag.c_str(), report->m_calls,
			totalPercentage.c_str(), totalTime.c_str(),
			selfPercentage.c_str(), selfTime.c_str());
	}
}

//------------------------------------------------------------------------
void ProfilerReport::ToggleView()
{
	if (m_view == REPORT_TREE_VIEW) {
		m_view = REPORT_FLAT_VIEW;
	}
	else if (m_view == REPORT_FLAT_VIEW) {
		m_view = REPORT_TREE_VIEW;
	}
}

//------------------------------------------------------------------------
void ProfilerReport::SwitchView(eReportView view)
{
	m_view = view;
}

//------------------------------------------------------------------------
void ProfilerReport::SetLastProfilerFrame(ProfilerNode_T* frame)
{
	if (CanProfileRun()) {
		m_latestFrame = frame;
		m_needsUpdate = true;
	}
}

//------------------------------------------------------------------------
void ProfilerReport::TraverseAndLogThroughTree(ProfilerNode_T* node, unsigned int tabs)
{
	if (node == nullptr) {
		return;
	}

	LogProfilerNode(node, tabs);

	TraverseAndLogThroughTree(node->m_firstChild, tabs + 1);
	TraverseAndLogThroughTree(node->m_rightSibling, tabs);
}

//------------------------------------------------------------------------
void ProfilerReport::TraverseAndAddReport(ProfilerNode_T* node, unsigned int treeHeight)
{
	if (node == nullptr) {
		return;
	}

	ProfilerReportNode_T* report = new ProfilerReportNode_T();
	ParseInformation(*report, node);
	report->m_treeHeight = treeHeight;
	m_treeReports.push_back(report);
	SortedInsertIntoFlatView(report);

	TraverseAndAddReport(node->m_firstChild, 1 + treeHeight);
	TraverseAndAddReport(node->m_rightSibling, treeHeight);
}

//------------------------------------------------------------------------
void ProfilerReport::LogProfilerNode(ProfilerNode_T* currentNode, unsigned int tabs)
{
	ProfilerReportNode_T reportNode;

	ParseInformation(reportNode, currentNode, tabs);

	std::string totalPercentage = Stringf("%3.2f%%", reportNode.m_totalPercentage);
	std::string totalTime = GetProperTimeSecondsString(reportNode.m_totalTime);
	std::string selfPercentage = Stringf("%3.2f%%", reportNode.m_selfPercent);
	std::string selfTime = GetProperTimeSecondsString(reportNode.m_selfTime);

	LogTaggedPrintf(REPORT_TAG, REPORT_LOG_FORMAT, reportNode.m_tag.c_str(), reportNode.m_calls,
		totalPercentage.c_str(), totalTime.c_str(),
		selfPercentage.c_str(), selfTime.c_str());
}

//------------------------------------------------------------------------
void ProfilerReport::ParseInformation(ProfilerReportNode_T& reportNode, ProfilerNode_T* node, unsigned int tabs)
{
	if (node) {
		// Name of the tag used in the push
		std::string tag = ParseTag(node, tabs);
		reportNode.m_tag = tag;

		// Number of times this tag was pushed
		unsigned int calls = node->m_callCount;
		reportNode.m_calls = calls;

		// The amount of time, as a percent of total frame time, is taken up by this node (including its children)
		double totalPercentageCalculation = CalculateTotalPercentageTime(node);
		reportNode.m_totalPercentage = totalPercentageCalculation;

		// Total time taken by this node (including its children)
		reportNode.m_totalTime = ParseTotalTime(node);

		// Time spent while this was the current active node
		double selfPercentageCalculation = CalculateSelfPercentageTime(node);
		reportNode.m_selfPercent = selfPercentageCalculation;

		// Same as selfPercentage, but as time
		reportNode.m_selfTime = ParseSelfTime(node);
	}
}

//------------------------------------------------------------------------
std::string ProfilerReport::ParseTag(ProfilerNode_T* node, unsigned int tabs)
{
	if (tabs != 0) {
		std::string formattedTag = Stringf("%s",node->m_tag);

		for (int i = 0; i < (int)tabs; ++i) {
			formattedTag = "   " + formattedTag;
		}

		return formattedTag;
	}
	else {
		return node->m_tag;
	}
}

//------------------------------------------------------------------------
double ProfilerReport::CalculateTotalPercentageTime(ProfilerNode_T* node)
{
	double nodeElapsedTimeSeconds = node->GetSelfElapsedTimeSeconds();
	double frameTime = ProfilerGetLastFrameTimeSeconds();
	double totalPercentageCalculation = (nodeElapsedTimeSeconds / frameTime) * 100;

	if (totalPercentageCalculation > 100) {
		totalPercentageCalculation = 100;
	}

	return totalPercentageCalculation;
}

//------------------------------------------------------------------------
double ProfilerReport::ParseTotalTime(ProfilerNode_T* node)
{
	std::string parsedTotalTime = "";

	double nodeElapsedTime = node->GetSelfElapsedTimeSeconds();

	return nodeElapsedTime;
}

//------------------------------------------------------------------------
double ProfilerReport::CalculateSelfPercentageTime(ProfilerNode_T* node)
{
	double nodeActiveTimeSeconds = node->GetSelfActiveTime();
	double frameTime = ProfilerGetLastFrameTimeSeconds();
	double totalPercentageCalculation = (nodeActiveTimeSeconds / frameTime) * 100;

	if (totalPercentageCalculation > 100) {
		totalPercentageCalculation = 100;
	}
	else if (totalPercentageCalculation <= 0) {
		totalPercentageCalculation = 0;
	}

	return totalPercentageCalculation;
}

//------------------------------------------------------------------------
double ProfilerReport::ParseSelfTime(ProfilerNode_T* node)
{
	std::string parsedSelfTime = "";

	double nodeSelfActiveTime = node->GetSelfActiveTime();

	if (nodeSelfActiveTime <= 0) {
		nodeSelfActiveTime = 0;
	}
	
	return nodeSelfActiveTime;
}

//------------------------------------------------------------------------
std::string ProfilerReport::GetProperTimeSecondsString(double time) const
{
	std::string parsedTime = "";

	if (time < 0.001) {
		time = ConvertSecondsToMicroseconds(time);
		return parsedTime = Stringf("%.4f us", time);
	}
	else if (time < 1) {
		time = ConvertSecondsToMilliseconds(time);
		return parsedTime = Stringf("%.4f ms", time);
	}
	else {
		return parsedTime = Stringf("%.4f s", time);
	}
}