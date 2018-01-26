#pragma once

#include "Engine/Core/StringUtils.hpp"

enum eReportView
{
	REPORT_TREE_VIEW,
	REPORT_FLAT_VIEW
};

enum eFlatViewSortCategory
{
	SORT_CALLS,
	SORT_TOTAL_PERCENT,
	SORT_TOTAL_TIME,
	SORT_SELF_PERCENT,
	SORT_SELF_TIME
};

struct ProfilerReportNode_T
{
	ProfilerReportNode_T() :
		m_tag(""),
		m_calls(0),
		m_totalPercentage(0),
		m_totalTime(0),
		m_selfPercent(0),
		m_selfTime(0),
		m_treeHeight(0)
	{};

	std::string m_tag;
	unsigned int m_calls;
	double m_totalPercentage;
	double m_totalTime;
	double m_selfPercent;
	double m_selfTime;
	unsigned int m_treeHeight;
};

const unsigned char MAX_FRAMETIME_HISTORY_SIZE = 50;

struct ProfilerNode_T;
class SimpleRenderer;

class ProfilerReport
{
public:
	ProfilerReport();
	ProfilerReport(ProfilerNode_T* frame);

public:
	void ProfilerUpdate(float deltaSeconds);
	void ProfilerSetRenderer(SimpleRenderer* renderer);
	void ProfilerRender() const;

public:
	void SetLatestFrame(ProfilerNode_T * frame);
	void Log();
	void ToggleView();
	void SwitchView(eReportView view);
	void SetLastProfilerFrame(ProfilerNode_T* frame);

public:
	ProfilerNode_T* m_latestFrame;

private:
	eReportView m_view;
	eFlatViewSortCategory m_flatViewDefaultSort;
	bool m_needsUpdate;
	ProfilerReportNode_T m_lastFramesReport;
	std::vector<ProfilerReportNode_T*> m_treeReports;
	std::vector<ProfilerReportNode_T*> m_flatReports;
	std::vector<double> m_frametimeHistory;

private:
	void DrawTextsForProfilerRendering() const;
	void RenderTreeViewText() const;
	void RenderFlatViewText() const;
	void DrawProfilerData(ProfilerReportNode_T* report, float textYPos) const;
	void DrawGraphForProfilerRendering() const;

private:
	void UpdateGraphicsMembers();
	void CleanUpOldReports();
	void AddFrameTimeToHistory(float deltaSeconds);
	void TraverseAndAddReport(ProfilerNode_T* node, unsigned int treeHeight = 0);
	void TraverseAndLogThroughTree(ProfilerNode_T* node, unsigned int tabs = 0);
	void SortedInsertIntoFlatView(ProfilerReportNode_T* report);
	void LogProfilerNode(ProfilerNode_T* node, unsigned int tabs = 0);
	void LogFlatReports(std::vector<ProfilerReportNode_T *> reports);
	void ParseInformation(ProfilerReportNode_T& reportNode, ProfilerNode_T* node, unsigned int tabs = 0);
	std::string ParseTag(ProfilerNode_T* node, unsigned int tabs);
	double CalculateTotalPercentageTime(ProfilerNode_T* node);
	double ParseTotalTime(ProfilerNode_T* node);
	std::string GetProperTimeSecondsString(double time) const;
	double CalculateSelfPercentageTime(ProfilerNode_T* node);
	double ParseSelfTime(ProfilerNode_T* node);
};