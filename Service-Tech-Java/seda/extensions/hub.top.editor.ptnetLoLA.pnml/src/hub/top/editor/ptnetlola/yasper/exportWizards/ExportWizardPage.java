/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 * ServiceTechnolog.org - PetriNet Editor Framework
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
 * The Original Code is this file as it was released on June 6, 2009.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.editor.ptnetlola.yasper.exportWizards;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Shell;

public class ExportWizardPage extends WizardPage implements Listener {
	
	// combo for the target directory
	private Combo templateDirCombo;
	
	// model name
	private String modelName;

	// listener (as a member field) to update the pageComplete-field
	// will be attached to any active element that influences the
	// "complete"-status of this page
	private ModifyListener modifyListener = new ModifyListener() {
		public void modifyText(ModifyEvent e) {
			setPageComplete(isPageComplete());
		}
	};

	
	public ExportWizardPage(String pageName) {
		super(pageName);
		// TODO Auto-generated constructor stub
		setTitle("title");
		setDescription("description");
	}

	public ExportWizardPage(String pageName, String title, ImageDescriptor titleImage) {
		super(pageName, title, titleImage);
		// TODO Auto-generated constructor stub
	}

	/**
	 * @see org.eclipse.jface.dialogs.IDialogPage#createControl(Composite)
	 */
	public void createControl(Composite parent) {
		// 3 minimal statements to create a wizard page:
		// (1) create a new child composite, (2) give it a layout
		Composite composite = new Composite(parent, SWT.NULL);
		composite.setLayout(new GridLayout(1,false));
		
		// some organizing: we need a shell for browsing to the target-dir
		final Shell shell = parent.getShell();
		
		// fill page with contents
		Group templateGroup = new Group(composite, SWT.NULL);
		templateGroup.setLayout(new GridLayout(1, false));
		templateGroup.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		templateGroup.setText("template group"); //$NON-NLS-1$

		Composite templateComposite = new Composite(templateGroup, SWT.NULL);
		templateComposite.setLayout(new GridLayout(3, false));
		templateComposite.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		
		// target directory controls
		templateDirCombo = new Combo(templateComposite, SWT.BORDER);
		templateDirCombo.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		String savedDir = "";
		if (savedDir != null) {
			templateDirCombo.add(savedDir);
			templateDirCombo.setText(savedDir);
		}
		templateDirCombo.addModifyListener(modifyListener);
		
		Button browseButton = new Button(templateComposite, SWT.NONE);
		browseButton.setText("Browse..."); //$NON-NLS-1$
		browseButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent event) {
				try {
					FileDialog dialog = new FileDialog (shell, SWT.NONE);
					// fill "save as..." dialogue with a suggested name
					dialog.setFileName(getOutputFileName("pnml"));
					String []ext = new String[] { "*.pnml" }; 
					dialog.setFilterExtensions(ext);
					
					String selectedFile = dialog.open();
					if (selectedFile != null) {
						templateDirCombo.add(selectedFile, 0);
						templateDirCombo.setText(selectedFile);
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
		// done: fill page with contents
		
		// 3 minimal statements to create a wizard page (cont.)
		// (3) set control, done
		setControl(composite);
	}
	
	protected String getModelName() {
		return modelName;
	}

	protected void setModelName(String modelName) {
		this.modelName = modelName;
	}
	
	/**
	 * replace model name extension with given extension
	 * @param ext
	 * @return suggested output filename
	 */
	private String getOutputFileName (String ext) {
		String out = modelName.substring(0,modelName.lastIndexOf("."));
		return out+"."+ext;
	}
	 
	/**
	 * @return the target file name into which the model shall be exported
	 */
	protected String getTargetFileName () {
		return templateDirCombo.getText().trim();
	}
	
	/**
	 * @see org.eclipse.swt.widgets.Listener#handleEvent(Event)
	 */
	public void handleEvent(Event event) {
		setPageComplete(isPageComplete());
		getWizard().getContainer().updateButtons();
	}
	
	/**
	 * return true if this page is completed
	 * 
	 * @see org.eclipse.jface.wizard.WizardPage#isComplete()
	 */
	public boolean isPageComplete () {
		return (templateDirCombo.getText().trim().length() > 0);
	}
}
