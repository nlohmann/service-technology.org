/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 * 		Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008
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

package hub.top.editor.eclipse.ui;

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
	private Combo exportFileNameBox;
	
	// model name
	private String originalResourceName;
	
	private String[] suggestedExtensions;

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
		setTitle("Export to file");
		setDescription("Choose the target file name for export.");
	}

	public ExportWizardPage(String pageName, String title, ImageDescriptor titleImage) {
		super(pageName, title, titleImage);
		// TODO Auto-generated constructor stub
	}

	/**
	 * @see org.eclipse.jface.dialogs.IDialogPage#createControl(Composite)
	 */
	public void createControl(Composite parent) {
		Composite composite = new Composite(parent, SWT.NULL);
		composite.setLayout(new GridLayout(1,false));
		
		final Shell shell = parent.getShell();
		
		Group exportFileNameGroup = new Group(composite, SWT.NULL);
		exportFileNameGroup.setLayout(new GridLayout(1, false));
		exportFileNameGroup.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		exportFileNameGroup.setText("Export to");

		Composite exportFileNameComposite = new Composite(exportFileNameGroup, SWT.NULL);
		exportFileNameComposite.setLayout(new GridLayout(3, false));
		exportFileNameComposite.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		
		// target directory controls
		exportFileNameBox = new Combo(exportFileNameComposite, SWT.BORDER);
		exportFileNameBox.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
		String savedDir = "";
		if (savedDir != null) {
			exportFileNameBox.add(savedDir);
			exportFileNameBox.setText(savedDir);
		}
		exportFileNameBox.addModifyListener(modifyListener);
		
		Button browseButton = new Button(exportFileNameComposite, SWT.NONE);
		browseButton.setText("Browse...");
		browseButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent event) {
				try {
					FileDialog dialog = new FileDialog (shell, SWT.NONE);
					// fill "save as..." dialogue with a suggested name
					dialog.setFileName(getSuggestedOutputFileName());
					dialog.setFilterExtensions(getSuggestExtensionFilter());
					String[] ext = getSuggestExtensionFilter();
					dialog.setFilterNames(ext);
					
					String selectedFile = dialog.open();
					if (selectedFile != null) {
					  
					  // append the selected file extension to the file name
	          String chosenExtension = suggestedExtensions[dialog.getFilterIndex()];
	          int lastDot = selectedFile.lastIndexOf('.');
	          if (lastDot == -1 || !selectedFile.substring(lastDot+1).equals(chosenExtension)) {
	            selectedFile += "."+chosenExtension;
	          }

						exportFileNameBox.add(selectedFile, 0);
						exportFileNameBox.setText(selectedFile);
					}
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
		
		

		setControl(composite);
	}
	
	protected String getOriginalResourceName() {
		return originalResourceName;
	}

	protected void setOriginalResourceName(String modelName) {
		this.originalResourceName = modelName;
	}
	
	/**
	 * replace original resource name extension with given extension
	 * @return suggested output filename
	 */
	private String getSuggestedOutputFileName () {
	  /*
		if (suggestedExtensions != null && suggestedExtensions.length > 0) {
		  String out = originalResourceName;
		  if (out.lastIndexOf(".") != -1) out = out.substring(0,out.lastIndexOf("."));
			return out+"."+suggestedExtensions[0];
		}
		else
		*/
			return originalResourceName;
	}
	
	/**
	 * @return a list of "*.EXT" entries for each suggested
	 * extension of the wizard
	 */
	private String[] getSuggestExtensionFilter () {
		if (suggestedExtensions != null && suggestedExtensions.length > 0) {
			String[] extensions = new String[suggestedExtensions.length];
			for (int i=0;i<suggestedExtensions.length;i++) {
				extensions[i] = "*."+suggestedExtensions[i];
			}
			return extensions;
		}
		return new String[] { "*.*" };
	}
	
	/**
	 * set an extension filter for the possible target file names,
	 * the first extension will be used as default extension for the export
	 * @param ext
	 */
	protected void setSuggestedExtensions (String[] ext) {
		suggestedExtensions = ext;
	}
	 
	/**
	 * @return the target file name into which the model shall be exported
	 */
	protected String getEnteredTargetFileName () {
		return exportFileNameBox.getText().trim();
	}
	
	/**
	 * @return the file format into which the model shall be exported
	 */
	protected String getTargetFileFormat() {
	  return "lola";
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
		return (exportFileNameBox.getText().trim().length() > 0);
	}
}
