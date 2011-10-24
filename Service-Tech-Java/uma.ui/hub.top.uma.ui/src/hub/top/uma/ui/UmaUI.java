package hub.top.uma.ui;

import hub.top.uma.Uma;

import java.io.PrintStream;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.console.ConsolePlugin;
import org.eclipse.ui.console.IConsole;
import org.eclipse.ui.console.IConsoleConstants;
import org.eclipse.ui.console.IConsoleView;
import org.eclipse.ui.console.MessageConsole;

public class UmaUI {
  
  private static UmaUI _ui = null;
  
  public static UmaUI getUI () {
    if (UmaUI._ui == null) {
      _ui = new UmaUI();
    }
    return _ui;
  }
  
  private MessageConsole messageConsole = null;
  
  public PrintStream out;
  
  public IProgressMonitor monitorHook = null;
  public String monitorHook_subTask = "";
  
  public UmaUI () {
    getMessageConsole();
    out = new PrintStream(messageConsole.newMessageStream()) {
      
      private String lineBuf = "";
      
      @Override
      public void print(String s) {
        if (monitorHook != null) {
          
          lineBuf += s;
          String printBuf;
          if (lineBuf.endsWith("\n")) {
            printBuf = lineBuf.substring(0, lineBuf.length()-1);
            lineBuf = "";
          } else {
            printBuf = lineBuf;
          }
          if (printBuf.length() > 80) {
            printBuf = printBuf.substring(printBuf.length() - 80);
          }
          monitorHook.subTask(monitorHook_subTask+"\n"+printBuf);
        }
        super.print(s);
      }
    };
  }
  
  public MessageConsole getMessageConsole() {
    if (messageConsole == null) {
     messageConsole = new MessageConsole("Uma", null);
     ConsolePlugin.getDefault().getConsoleManager().addConsoles(
       new IConsole[] { messageConsole });
    }

    return messageConsole;
  }
  
  public void showMessageConsole() {
      PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable() {
        
        @Override
        public void run() {
          try {
            // TODO Auto-generated method stub
            IConsole umaConsole = getMessageConsole();//your console instance
            IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();//obtain the active page
            String id = IConsoleConstants.ID_CONSOLE_VIEW;
            IConsoleView view = (IConsoleView) page.showView(id);
            view.display(umaConsole);
          } catch (PartInitException e) {
            Activator.getPluginHelper().logWarning("Could not open console view for Uma.");
          }
      }
    });
  }
  
  public void switchOutStream_uiConsole() {
    showMessageConsole();
    Uma.out = this.out;
  }
  
  public void switchOutStream_System() {
    showMessageConsole();
    Uma.out = System.out;
  }
}
