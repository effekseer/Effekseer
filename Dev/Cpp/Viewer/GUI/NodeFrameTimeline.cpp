#include "../3rdParty/imgui_addon/ImGuizmo/ImSequencer.h"
#include "NodeFrameTimeline.h"

static const char* SequencerItemTypeNames[] = { "Camera","Music", "ScreenEffect", "FadeIn", "Animation" };

struct MySequence : public ImSequencer::SequenceInterface
{
    // interface with sequencer

    virtual int GetFrameMin() const {
        return mFrameMin;
    }
    virtual int GetFrameMax() const {
        return mFrameMax;
    }
    virtual int GetItemCount() const { return (int)myItems.size(); }

    virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
    
    virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }

    virtual const char* GetItemLabel(int index) const
    {
        return myItems[index].name.c_str();
    }

    virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
    {
        MySequenceItem& item = myItems[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (start)
            *start = &item.mFrameStart;
        if (end)
            *end = &item.mFrameEnd;
        if (type)
            *type = item.mType;
    }
    virtual void Add(int type) { myItems.push_back(MySequenceItem{ type, 0, 10, false }); };
    virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
    virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }

    virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }

    // my datas
    MySequence() : mFrameMin(0), mFrameMax(0) {}
    int mFrameMin, mFrameMax;
    struct MySequenceItem
    {
        int mType;
        int mFrameStart, mFrameEnd;
        bool mExpanded;
        std::string name;
    };
    std::vector<MySequenceItem> myItems;
    //RampEdit rampEdit;

    virtual void DoubleClick(int index) {
        if (myItems[index].mExpanded)
        {
            myItems[index].mExpanded = false;
            return;
        }
        for (auto& item : myItems)
            item.mExpanded = false;
        myItems[index].mExpanded = !myItems[index].mExpanded;
    }

    virtual void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
    {
        //static const char* labels[] = { "Translation", "Rotation" , "Scale" };

        //rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
        //rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
        //draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
        //for (int i = 0; i < 3; i++)
        //{
        //    ImVec2 pta(legendRect.Min.x + 30, legendRect.Min.y + i * 14.f);
        //    ImVec2 ptb(legendRect.Max.x, legendRect.Min.y + (i + 1) * 14.f);
        //    draw_list->AddText(pta, rampEdit.mbVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);
        //    if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
        //        rampEdit.mbVisible[i] = !rampEdit.mbVisible[i];
        //}
        //draw_list->PopClipRect();

        //ImGui::SetCursorScreenPos(rc.Min);
        //ImCurveEdit::Edit(rampEdit, rc.Max - rc.Min, 137 + index, &clippingRect);
    }

    virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
    {
        //rampEdit.mMax = ImVec2(float(mFrameMax), 1.f);
        //rampEdit.mMin = ImVec2(float(mFrameMin), 0.f);
        //draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
        //for (int i = 0; i < 3; i++)
        //{
        //    for (int j = 0; j < rampEdit.mPointCount[i]; j++)
        //    {
        //        float p = rampEdit.mPts[i][j].x;
        //        if (p < myItems[index].mFrameStart || p > myItems[index].mFrameEnd)
        //            continue;
        //        float r = (p - mFrameMin) / float(mFrameMax - mFrameMin);
        //        float x = ImLerp(rc.Min.x, rc.Max.x, r);
        //        draw_list->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xAA000000, 4.f);
        //    }
        //}
        //draw_list->PopClipRect();
    }
};

static MySequence mySequence;
static int selectedEntry = -1;
static int firstFrame = 0;
static bool expanded = true;
static int currentFrame = 100;

bool NodeFrameTimeline::BeginNodeFrameTimeline()
{
    mySequence.myItems.clear();
    mySequence.mFrameMin = -100;    // TODO: test
    mySequence.mFrameMax = 1000;    // TODO: test

    return true;
}

void NodeFrameTimeline::TimelineNode(const char* title)
{
    mySequence.myItems.push_back(MySequence::MySequenceItem{ 0, 10, 30, false, title });
}

void NodeFrameTimeline::EndNodeFrameTimeline()
{

    ImGui::PushItemWidth(130);
    ImGui::InputInt("Frame Min", &mySequence.mFrameMin);
    ImGui::SameLine();
    ImGui::InputInt("Frame ", &currentFrame);
    ImGui::SameLine();
    ImGui::InputInt("Frame Max", &mySequence.mFrameMax);
    ImGui::PopItemWidth();
    Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL /*| ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME*/);
    // add a UI to edit that particular item
    if (selectedEntry != -1)
    {
        const MySequence::MySequenceItem& item = mySequence.myItems[selectedEntry];
        ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
        // switch (type) ....
    }

#if 0



    int id = 9999;

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
    {
        return false;
    }


    ImGui::Columns(2);

    if (ImGui::BeginChild("###BeginNodeFrameTimeline_Nodeist")) {

        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

        static int selection_mask = (1 << 2);
        int node_clicked = -1;
        for (int i = 0; i < 3; i++)
        {
            // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
            ImGuiTreeNodeFlags node_flags = base_flags;
            const bool is_selected = (selection_mask & (1 << i)) != 0;
            if (is_selected)
                node_flags |= ImGuiTreeNodeFlags_Selected;


            {
                // Items 0..2 are Tree Node
                bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                if (ImGui::IsItemClicked())
                    node_clicked = i;
                //if (test_drag_and_drop && ImGui::BeginDragDropSource())
                //{
                //    ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                //    ImGui::Text("This is a drag and drop source");
                //    ImGui::EndDragDropSource();
                //}
                if (node_open)
                {
                    ImGui::BulletText("Blah blah\nBlah Blah");
                    ImGui::TreePop();
                }
            }
        }


        ImGui::EndChild();
    }


    ImGui::NextColumn();


    const ImRect innerRect = window->InnerRect;
    float width = innerRect.Max.x - innerRect.Min.x;
    float height = innerRect.Max.y - innerRect.Min.y;

    //ImVec2 size(500, 500);
    ImGui::SetNextWindowContentSize(ImVec2(1500.0f, 0.0f));
    if (ImGui::BeginChild(id, ImVec2(0, 0), /*ImGuiWindowFlags_NoScrollbar |*/ ImGuiWindowFlags_HorizontalScrollbar))
    {
        ImVec2 op = ImGui::GetCursorScreenPos();

        float itemHeight = ImGui::GetFrameHeightWithSpacing();
        for (auto y = 0; y < 3; y++)
        {
            window->DrawList->AddLine(ImVec2(op.x, op.y + itemHeight * y), ImVec2(op.x + width - 100.f, op.y + itemHeight * y), 0xFF0000FF);
        }

        //ImDrawList* draw_list = ImGui::GetForegroundDrawList();
        //ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //draw_list->AddLine(ImVec2(0, 0), ImVec2(1000, 1000), 0xFF0000FF, 3);

        ImGui::EndChild();
    }



    //ImGui::SetNextWindowContentSize(ImVec2(1500.0f, 0.0f));
    //ImVec2 child_size = ImVec2(0, ImGui::GetFontSize() * 20.0f);
    //ImGui::BeginChild("##ScrollingRegion", child_size, false, ImGuiWindowFlags_HorizontalScrollbar);
    //ImGui::Columns(10);
    //int ITEMS_COUNT = 2000;
    //ImGuiListClipper clipper(ITEMS_COUNT);  // Also demonstrate using the clipper for large list
    //while (clipper.Step())
    //{
    //    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    //        for (int j = 0; j < 10; j++)
    //        {
    //            ImGui::Text("Line %d Column %d...", i, j);
    //            ImGui::NextColumn();
    //        }
    //}
    //ImGui::Columns(1);
#endif
}
