#include "gui_organizer.h"
#include "../src/helpers.h"

void GUI_Organizer::set_reductionLevel(int value)
{
  if (value != reduction_level)
  {
    reduction_level = value;
    
    set_commandLine();
    
    switch (reduction_level)
    {
      case(0): emit reductionLevel_changed("none"); break;
      case(1): emit reductionLevel_changed("remove dead nodes"); break;
      case(2): emit reductionLevel_changed("remove unused status places"); break;
      case(3): emit reductionLevel_changed("remove identical nodes"); break;
      case(4): emit reductionLevel_changed("remove serial nodes"); break;
      case(5): emit reductionLevel_changed("remove loop nodes"); break;
      case(6): emit reductionLevel_changed("remove equal nodes"); break;
    }
  }
}

void GUI_Organizer::set_commandLine()
{
  string temp = "bpel2owfn";
  
  if (reduction_level > 0)
    temp += " -r" + toString(reduction_level);
  
  emit commandLine_changed(temp.c_str());
}