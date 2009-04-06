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

package hub.top.editor.lola.text;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.impl.ResourceImpl;
import org.osgi.framework.Bundle;



import hub.sam.tef.layout.AbstractLayoutManager;
import hub.sam.tef.layout.BlockLayout;
import hub.sam.tef.modelcreating.IModelCreatingContext;
import hub.top.editor.eclipse.EditorUtil;
import hub.top.editor.lola.text.Activator;
import hub.top.editor.lola.text.modelcreating.LolaModelCreatingContext;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.provider.PtnetLoLAItemProviderAdapterFactory;

public class ModelEditor extends hub.sam.tef.editor.model.ModelEditor implements hub.top.editor.eclipse.IFrameWorkEditor {

	@Override
	public EPackage[] createMetaModelPackages() {
		return new EPackage[] { PtnetLoLAPackage.eINSTANCE };
	}

	@Override
	protected Bundle getPluginBundle() {
		return Activator.getDefault().getBundle();
	}

	@Override
	protected String getSyntaxPath() {
		return "/resources/ptnetLoLA.etslt";
	}

	@Override
	public AdapterFactory[] createItemProviderAdapterFactories() {
		return new AdapterFactory[] { new PtnetLoLAItemProviderAdapterFactory() };
	}
	
	@Override
	public AbstractLayoutManager createLayout() {	
		return new BlockLayout();
	}
	
	@Override
	public IModelCreatingContext createModelCreatingContext() {
		return new LolaModelCreatingContext(
				getMetaModelPackages(), getSemanticsProvider(), 
				new ResourceImpl(), getCurrentText());
	}
	
	public void refresh() {
		
	}
	
	/**
	 * wrap this editor in a {@link EditorUtil}
	 * @return editor util wrapping this editor
	 */
	public EditorUtil getEditorUtil() {
		return new EditorUtil(this) {

			/* (non-Javadoc)
			 * @see hub.top.editor.gef.EditorUtil#getCurrentResource()
			 */
			public Resource getCurrentResource() {
				return ((ModelEditor)getEditor()).getCurrentModel();

			}
			
			/*
			 * (non-Javadoc)
			 * @see hub.top.editor.eclipse.EditorUtil#defaultFileExtensions()
			 */
			@Override
			public String defaultFileExtension() {
				return "ptnet";
			}
			
			@Override
			public void refreshEditorView() {
				ModelEditor editor = (ModelEditor)getEditor();
				hub.sam.tef.editor.text.FormatAction act = new hub.sam.tef.editor.text.FormatAction(editor);
				act.run();
			}
			
			public String getCurrentText() {
				return ((ModelEditor)getEditor()).getCurrentPrettyPrintedText();
			}
			
			@Override
			public String[] compatibleTextFileExtensions() {
				return new String[] {"lola"};
			}
		};
	}
}
