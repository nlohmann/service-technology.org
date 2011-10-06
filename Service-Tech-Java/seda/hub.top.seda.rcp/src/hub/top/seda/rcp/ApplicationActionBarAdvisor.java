/*****************************************************************************\
 * Copyright (c) 2009 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - RCP Application 
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is this file as it was released on April 06, 2009.
 * The Initial Developer of the Original Code are
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
\*****************************************************************************/

package hub.top.seda.rcp;

import hub.top.seda.rcp.ui.SwitchWorkSpaceAction;

import org.eclipse.jface.action.GroupMarker;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.ICoolBarManager;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.IStatusLineManager;
import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.action.Separator;
import org.eclipse.jface.action.ToolBarContributionItem;
import org.eclipse.jface.action.ToolBarManager;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IWorkbenchActionConstants;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.actions.ActionFactory;
import org.eclipse.ui.actions.ContributionItemFactory;
import org.eclipse.ui.application.ActionBarAdvisor;
import org.eclipse.ui.application.IActionBarConfigurer;

public class ApplicationActionBarAdvisor extends ActionBarAdvisor {

    public ApplicationActionBarAdvisor(IActionBarConfigurer configurer) {
        super(configurer);
    }
    
    protected void makeActions(IWorkbenchWindow window) {
    	
    	register(ActionFactory.NEW_WIZARD_DROP_DOWN.create(window));
    	register(ActionFactory.CLOSE.create(window));
    	register(ActionFactory.SAVE.create(window));
    	register(ActionFactory.SAVE_AS.create(window));
    	register(ActionFactory.SAVE_ALL.create(window));
    	register(ActionFactory.PRINT.create(window));
    	register(ActionFactory.EXPORT.create(window));
    	register(ActionFactory.QUIT.create(window));
    	
    	register(ActionFactory.UNDO.create(window));
    	register(ActionFactory.REDO.create(window));
    	register(ActionFactory.COPY.create(window));
    	register(ActionFactory.CUT.create(window));
    	register(ActionFactory.PASTE.create(window));
    	
    	register(ActionFactory.DELETE.create(window));

    	
    	register(ActionFactory.RESET_PERSPECTIVE.create(window));
    	register(ActionFactory.PREFERENCES.create(window));
    	
    	if (window.getWorkbench().getIntroManager().hasIntro())
    		register(ActionFactory.INTRO.create(window));
    	
    	register(ActionFactory.HELP_CONTENTS.create(window));
    	register(ActionFactory.ABOUT.create(window));
    }

    protected void fillMenuBar(IMenuManager menuBar) {
    	
    	super.fillMenuBar(menuBar);

    	final IWorkbenchWindow window =
    		getActionBarConfigurer().getWindowConfigurer().getWindow();
    	
    	// File -----------------------------------------------
    	MenuManager fileMenu = new MenuManager("&File", IWorkbenchActionConstants.M_FILE);
    	menuBar.add(fileMenu);

    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.FILE_START));
    	
    	final String NEW_ID = ActionFactory.NEW.getId();
    	MenuManager newMenu = new MenuManager("New...", NEW_ID);
    	//newMenu.add(new Separator(NEW_ID));
    	final IContributionItem newWizards = 
    		ContributionItemFactory.NEW_WIZARD_SHORTLIST.create(window);
    	newMenu.add(newWizards);
    	newMenu.add(new GroupMarker(IWorkbenchActionConstants.NEW_EXT));

    	fileMenu.add(newMenu);
    	// need additions marker for placement of "open file..."
    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.MB_ADDITIONS));
    	
    	fileMenu.add(new Separator());
    	fileMenu.add(getAction(ActionFactory.CLOSE.getId()));
    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.CLOSE_EXT));
    	
    	fileMenu.add(new Separator());
    	fileMenu.add(getAction(ActionFactory.SAVE.getId()));
    	fileMenu.add(getAction(ActionFactory.SAVE_AS.getId()));
    	fileMenu.add(getAction(ActionFactory.SAVE_ALL.getId()));
    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.SAVE_EXT));
    	
    	fileMenu.add(new Separator());
    	fileMenu.add(getAction(ActionFactory.PRINT.getId()));
    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.PRINT_EXT));
    	
    	fileMenu.add(new Separator());
    	fileMenu.add(getAction(ActionFactory.EXPORT.getId()));
    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.IMPORT_EXT));
    	
    	fileMenu.add(new Separator());
    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.WB_START));
    	fileMenu.add(new SwitchWorkSpaceAction(null));
    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.WB_END));
    	
    	fileMenu.add(new Separator());
    	fileMenu.add(getAction(ActionFactory.QUIT.getId()));
    	fileMenu.add(new GroupMarker(IWorkbenchActionConstants.FILE_END));
    	
    	// Edit -----------------------------------------------
    	MenuManager editMenu = new MenuManager("&Edit",	IWorkbenchActionConstants.M_EDIT);
    	menuBar.add(editMenu);
    	editMenu.add(new GroupMarker(IWorkbenchActionConstants.EDIT_START));
    	editMenu.add(getAction(ActionFactory.UNDO.getId()));
    	editMenu.add(getAction(ActionFactory.REDO.getId()));
    	editMenu.add(new GroupMarker(IWorkbenchActionConstants.UNDO_EXT));
    	editMenu.add(new Separator());
    	editMenu.add(getAction(ActionFactory.COPY.getId()));
    	editMenu.add(getAction(ActionFactory.CUT.getId()));
    	editMenu.add(getAction(ActionFactory.PASTE.getId()));
    	editMenu.add(new GroupMarker(IWorkbenchActionConstants.FIND_EXT));
    	editMenu.add(new GroupMarker(IWorkbenchActionConstants.EDIT_END));
    	
    	// additions to the menu bar go here, before the windows menu
    	menuBar.add(new GroupMarker(IWorkbenchActionConstants.MB_ADDITIONS));

    	// Window
    	MenuManager windowMenu = new MenuManager("&Window", IWorkbenchActionConstants.M_WINDOW);
    	menuBar.add(windowMenu);
    	MenuManager perspMenu = new MenuManager("&Open Perspective");
    	final IContributionItem perspShortList =
    		ContributionItemFactory.PERSPECTIVES_SHORTLIST.create(window);
    	perspMenu.add(perspShortList);
    	windowMenu.add(perspMenu);
    	
    	MenuManager viewMenu = new MenuManager("&Show View");
    	final IContributionItem viewShortList =
    		ContributionItemFactory.VIEWS_SHORTLIST.create(window);
    	viewMenu.add(viewShortList);
    	windowMenu.add(viewMenu);
    	
    	
    	windowMenu.add(new Separator());
    	windowMenu.add(getAction(ActionFactory.RESET_PERSPECTIVE.getId()));
    	windowMenu.add(new GroupMarker(IWorkbenchActionConstants.WINDOW_EXT));
    	
    	windowMenu.add(new Separator());
    	windowMenu.add(getAction(ActionFactory.PREFERENCES.getId()));
    	
    	// Help
    	MenuManager helpMenu = new MenuManager("&Help", IWorkbenchActionConstants.M_HELP);
    	menuBar.add(helpMenu);
    	helpMenu.add(new GroupMarker(IWorkbenchActionConstants.HELP_START));
    	IAction introAction = getAction(ActionFactory.INTRO.getId());
    	if (introAction != null)
    		helpMenu.add(introAction);
    	helpMenu.add(getAction(ActionFactory.HELP_CONTENTS.getId()));
    	helpMenu.add(new Separator("update"));
    	helpMenu.add(new GroupMarker(IWorkbenchActionConstants.HELP_END));
    	helpMenu.add(getAction(ActionFactory.ABOUT.getId()));
    }
    
    @Override
    protected void fillCoolBar(ICoolBarManager coolBar) {
    	// TODO Auto-generated method stub
    	//super.fillCoolBar(coolBar);
    	coolBar.removeAll();
    	
    	// create file bar 
    	IToolBarManager fileBar = new ToolBarManager(SWT.FLAT | SWT.RIGHT);
    	fileBar.add(new GroupMarker(IWorkbenchActionConstants.FILE_START));
    	fileBar.add(getAction(ActionFactory.SAVE.getId()));
    	fileBar.add(new GroupMarker(IWorkbenchActionConstants.FILE_END));
    	
    	// Add the entries to the toolbar
    	coolBar.add(new ToolBarContributionItem(fileBar, "file"));
    	coolBar.add(new GroupMarker(IWorkbenchActionConstants.MB_ADDITIONS));
    }
    
    @Override
    protected void fillStatusLine(IStatusLineManager statusLine) {
    	//statusLine.add(getAction(ActionFactory.OPEN_NEW_WINDOW.getId()));
    }
   
}
