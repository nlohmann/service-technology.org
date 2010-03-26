/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 *  		Dirk Fahland
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
 */
package hub.top.lang.flowcharts.diagram.part;

import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.edit.provider.IWrapperItemProvider;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryContentProvider;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.wizard.WizardPage;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;

/**
 * Wizard page that allows to select element from model.
 * @generated
 */
public class ModelElementSelectionPage extends WizardPage {
  /**
   * @generated
   */
  protected EObject selectedModelElement;

  /**
   * @generated
   */
  private TreeViewer modelViewer;

  /**
   * @generated
   */
  public ModelElementSelectionPage(String pageName) {
    super(pageName);
  }

  /**
   * @generated
   */
  public EObject getModelElement() {
    return selectedModelElement;
  }

  /**
   * @generated
   */
  public void setModelElement(EObject modelElement) {
    selectedModelElement = modelElement;
    if (modelViewer != null) {
      if (selectedModelElement != null) {
        modelViewer.setInput(selectedModelElement.eResource());
        modelViewer.setSelection(new StructuredSelection(selectedModelElement));
      } else {
        modelViewer.setInput(null);
      }
      setPageComplete(validatePage());
    }
  }

  /**
   * @generated
   */
  public void createControl(Composite parent) {
    initializeDialogUnits(parent);

    Composite plate = new Composite(parent, SWT.NONE);
    plate.setLayoutData(new GridData(GridData.FILL_BOTH));
    GridLayout layout = new GridLayout();
    layout.marginWidth = 0;
    plate.setLayout(layout);
    setControl(plate);

    Label label = new Label(plate, SWT.NONE);
    label.setText(getSelectionTitle());
    label.setLayoutData(new GridData(GridData.HORIZONTAL_ALIGN_BEGINNING));

    modelViewer = new TreeViewer(plate, SWT.SINGLE | SWT.H_SCROLL
        | SWT.V_SCROLL | SWT.BORDER);
    GridData layoutData = new GridData(GridData.FILL_BOTH);
    layoutData.heightHint = 300;
    layoutData.widthHint = 300;
    modelViewer.getTree().setLayoutData(layoutData);
    modelViewer.setContentProvider(new AdapterFactoryContentProvider(
        FlowchartDiagramEditorPlugin.getInstance()
            .getItemProvidersAdapterFactory()));
    modelViewer.setLabelProvider(new AdapterFactoryLabelProvider(
        FlowchartDiagramEditorPlugin.getInstance()
            .getItemProvidersAdapterFactory()));
    if (selectedModelElement != null) {
      modelViewer.setInput(selectedModelElement.eResource());
      modelViewer.setSelection(new StructuredSelection(selectedModelElement));
    }
    modelViewer.addSelectionChangedListener(new ISelectionChangedListener() {
      public void selectionChanged(SelectionChangedEvent event) {
        ModelElementSelectionPage.this
            .updateSelection((IStructuredSelection) event.getSelection());
      }
    });

    setPageComplete(validatePage());
  }

  /**
   * Override to provide custom model element description.
   * @generated
   */
  protected String getSelectionTitle() {
    return Messages.ModelElementSelectionPageMessage;
  }

  /**
   * @generated
   */
  protected void updateSelection(IStructuredSelection selection) {
    selectedModelElement = null;
    if (selection.size() == 1) {
      Object selectedElement = selection.getFirstElement();
      if (selectedElement instanceof IWrapperItemProvider) {
        selectedElement = ((IWrapperItemProvider) selectedElement).getValue();
      }
      if (selectedElement instanceof FeatureMap.Entry) {
        selectedElement = ((FeatureMap.Entry) selectedElement).getValue();
      }
      if (selectedElement instanceof EObject) {
        selectedModelElement = (EObject) selectedElement;
      }
    }
    setPageComplete(validatePage());
  }

  /**
   * Override to provide specific validation of the selected model element.
   * @generated
   */
  protected boolean validatePage() {
    return true;
  }

}
