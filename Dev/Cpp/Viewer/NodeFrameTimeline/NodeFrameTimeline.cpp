#include "NodeFrameTimeline.h"

bool NodeFrameTimeline::BeginNodeFrameTimeline()
{
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
	return true;
}

void NodeFrameTimeline::EndNodeFrameTimeline()
{
    //ImGui::EndChild();
}
