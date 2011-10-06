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
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.impl.ResourceImpl;
import org.osgi.framework.Bundle;



import hub.sam.tef.Utilities;
import hub.sam.tef.layout.AbstractLayoutManager;
import hub.sam.tef.layout.BlockLayout;
import hub.sam.tef.modelcreating.IModelCreatingContext;
import hub.sam.tef.modelcreating.ModelCreatingException;
import hub.sam.tef.prettyprinting.PrettyPrintState;
import hub.sam.tef.prettyprinting.PrettyPrinter;
import hub.sam.tef.semantics.DefaultIdentificationScheme;
import hub.sam.tef.semantics.DefaultSemanticsProvider;
import hub.sam.tef.semantics.ISemanticsProvider;
import hub.sam.tef.tsl.Syntax;
import hub.sam.tef.tsl.TslException;
import hub.top.editor.eclipse.EditorUtil;
import hub.top.editor.lola.text.Activator;
import hub.top.editor.lola.text.modelcreating.LolaModelCreatingContext;
import hub.top.editor.lola.text.prettyprinting.PrettyPrinterTPN;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.provider.PtnetLoLAItemProviderAdapterFactory;

public class ModelEditorTPN extends hub.top.editor.ModelEditor implements hub.top.editor.eclipse.IFrameWorkEditor {
  
  /**
   * @return
   *    references to all EMF packages this editor needs for
   *    parsing and writing its files
   */
  public static EPackage[] createMetaModelPackages_default() {
    return new EPackage[] { PtnetLoLAPackage.eINSTANCE };
  }

  /**
   * @return
   *    reference to the Bundle of this plugin for referencing
   *    resources files of this editor, e.g. grammar descriptions
   * @see TextEditor#getSyntaxPath_default()
   */
  public static Bundle getPluginBundle_default() {
    return Activator.getDefault().getBundle();
  }
  
  /**
   * @return
   *    location of the grammar description relative to this plugin
   * @see TextEditor#getPluginBundle_default()
   */
  public static String getSyntaxPath_default() {
    return "/resources/ptnetTPN.etslt";
  }
  
  /**
   * @return syntax model for this editor
   */
  public static Syntax createSyntax_default() {
    try {
      Syntax syntax = Utilities.loadSyntaxDescription(
          getPluginBundle_default(),
          getSyntaxPath_default(),
          createMetaModelPackages_default());
      return syntax;
    } catch (TslException e) {
        Activator.getPluginHelper().logError("Failed to read syntax description: "+ModelEditor.getSyntaxPath_default(), e);
    }
    return null;
  }
  
  /**
   * @return standard layout manager for pretty printing models 
   */
  public static AbstractLayoutManager createLayout_default() {
    return new BlockLayout();  
  }
  
  /**
   * @param model
   * @return
   *    a pretty printed text representation of the <code>model</code>
   */
  public static String getText(EObject model) {
    PrettyPrinter pp = new PrettyPrinter(
        createSyntax_default(),
        new DefaultSemanticsProvider(DefaultIdentificationScheme.INSTANCE));
    pp.setLayout(createLayout_default());
    
    try {
      PrettyPrintState state = pp.print(model);
      return state.toString();
    } catch (ModelCreatingException e) {
      Activator.getPluginHelper().logError("Failed to print model.", e);
    }
    
    return null;
  }

	@Override
	public EPackage[] createMetaModelPackages() {
		return createMetaModelPackages_default();
	}

	@Override
	protected Bundle getPluginBundle() {
		return getPluginBundle_default();
	}

	@Override
	protected String getSyntaxPath() {
		return getSyntaxPath_default();
	}

	@Override
	public AdapterFactory[] createItemProviderAdapterFactories() {
		return new AdapterFactory[] { new PtnetLoLAItemProviderAdapterFactory() };
	}
	
	@Override
	public AbstractLayoutManager createLayout() {	
		return createLayout_default();
	}
	
	@Override
	public IModelCreatingContext createModelCreatingContext() {
		return new LolaModelCreatingContext(
				getMetaModelPackages(), getSemanticsProvider(), 
				new ResourceImpl(), getCurrentText());
	}
	
	@Override
	public PrettyPrinter createPrettyPrinter(Syntax syntax,
			ISemanticsProvider semanticsProvider) {
		return new PrettyPrinterTPN(syntax, semanticsProvider);
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
				return ((ModelEditorTPN)getEditor()).getCurrentModel();
			}
			
      @Override
      public EList<EObject> getCurrentModel() {
        Resource res = getCurrentResource();
        if (res != null) return res.getContents();
        return null;
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
				ModelEditorTPN editor = (ModelEditorTPN)getEditor();
				hub.sam.tef.editor.text.FormatAction act = new hub.sam.tef.editor.text.FormatAction(editor);
				act.run();
			}
			
			@Override
			public String[] compatibleTextFileExtensions() {
				return new String[] {"tpn"};
			}
		};
	}
}
