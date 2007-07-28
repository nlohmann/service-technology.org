#include "gui_organizer.h"
#include "../src/helpers.h" // for toString(int)

#include <QProcess>
#include <QErrorMessage>


/******************************************************************************
 * CONSTRUCTOR
 ******************************************************************************/

GUI_Organizer::GUI_Organizer() :
  reduction_level(0),
  use_long_options(false),
  fileFormat_fiona(false),
  fileFormat_lola(false),
  fileFormat_pnml(false),
  fileFormat_dot(false),
  output_filename(""),
  input_filename(""),
  command_line(""),
  patterns(1),
  patterns_variables(false)
{
}





/******************************************************************************
* SLOTS
******************************************************************************/

void GUI_Organizer::translateButton_pressed()
{
//  QProcess run;
//  run.start(QString(command_line.c_str()));
//  QByteArray result = run.readAllStandardError();
  
  emit show_errormessage("Well...<br>This feature is not yet implemented...");
}


void GUI_Organizer::set_patternVariables(bool value)
{
  patterns_variables = value;
  set_commandLine();
}

void GUI_Organizer::set_patternSmall(bool value)
{
  if (value)
  {
    patterns = 1;
    set_commandLine();
    emit no_variables_possible(false);
  }
}

void GUI_Organizer::set_patternNofhfaults(bool value)
{
  if (value)
  {
    patterns = 2;
    set_commandLine();
    emit no_variables_possible(true);
  }
}

void GUI_Organizer::set_patternStandardfaults(bool value)
{
  if (value)
  {
    patterns = 3;
    set_commandLine();
    emit no_variables_possible(true);
  }
}

void GUI_Organizer::set_outputFileName(QString value)
{
  output_filename = value.toStdString();
  
  set_commandLine();
}


void GUI_Organizer::set_inputFileName(QString value)
{
  input_filename = value.toStdString();
  
  set_commandLine();
}


void GUI_Organizer::set_fileFormatFiona(bool value)
{
  fileFormat_fiona = value;
  
  set_commandLine();
}

void GUI_Organizer::set_fileFormatLola(bool value)
{
  fileFormat_lola = value;
  
  set_commandLine();
}
void GUI_Organizer::set_fileFormatPnml(bool value)
{
  fileFormat_pnml = value;
  
  set_commandLine();
}

void GUI_Organizer::set_fileFormatDot(bool value)
{
  fileFormat_dot = value;
  
  set_commandLine();
}



void GUI_Organizer::set_longOptions(bool value)
{
  use_long_options = value;
  
  set_commandLine();
}





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
  
  
  // the input file
  if (use_long_options)
    temp += " --input=" + input_filename;
  else
    temp += " -i" + input_filename;
  
  
  // the mode
  if (use_long_options)
    temp += " --modus=petrinet";
  else
    temp += " -mpetrinet";

  
  // the pattern parameters
  if (patterns > 1)
  {
    if (use_long_options)
      temp += " --parameter=";
    else
      temp += " -p";
    
    switch (patterns)
    {
      case(2): temp += "nofhfaults"; break;
      case(3): temp += "standardfaults"; break;
    }
    
    if (patterns_variables)
    {
      if (use_long_options)
        temp += " --parameter=variables";
      else
        temp += " -pvariables";
    }    
  }
  
  // the file formats
  if (fileFormat_fiona)
  {
    if (use_long_options)
      temp += " --format=owfn";
    else
      temp += " -fowfn";
  }
  
  if (fileFormat_lola)
  {
    if (use_long_options)
      temp += " --format=lola";
    else
      temp += " -flola";
  }
  
  if (fileFormat_pnml)
  {
    if (use_long_options)
      temp += " --format=pnml";
    else
      temp += " -fowfn";
  }
  
  if (fileFormat_dot)
  {
    if (use_long_options)
      temp += " --format=dot";
    else
      temp += " -fowfn";
  }
  
  
  // the reduction levels
  if (reduction_level > 0)
  {
    if (use_long_options)
      temp += " --reduce=" + toString(reduction_level);
    else
      temp += " -r" + toString(reduction_level);
  }
  
  
  if (output_filename != "")
    temp += " --output=" + output_filename;
  else
    temp += " -o";
  
  
  command_line = temp;
  
  emit commandLine_changed(temp.c_str());
}