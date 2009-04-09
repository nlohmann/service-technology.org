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
 * The Original Code is this file as it was released on April 07, 2009.
 * The Initial Developer of the Original Code is
 * 		IBM Corporation
 * 
 * Portions created by the Initial Developer are Copyright (c) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Dirk Fahland
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

package hub.top.editor.petrinets.diagram.graphics;

import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;
import hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin;
import hub.top.editor.ptnetLoLA.util.PtnetLoLAValidator;

import java.net.URL;

import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.gmf.runtime.diagram.core.listener.DiagramEventBroker;
import org.eclipse.gmf.runtime.diagram.core.listener.NotificationListener;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.services.decorator.IDecoration;
import org.eclipse.gmf.runtime.diagram.ui.services.decorator.IDecorator;
import org.eclipse.gmf.runtime.diagram.ui.services.decorator.IDecoratorTarget;
import org.eclipse.gmf.runtime.notation.DescriptionStyle;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.Node;
import org.eclipse.gmf.runtime.notation.NotationPackage;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;
import org.omg.PortableInterceptor.SUCCESSFUL;

/**
 */
public class NodeDecorator implements IDecorator { 

	/** the object to be decorated */
	private IDecoratorTarget decoratorTarget;

	/** the decoration being displayed */
	private IDecoration decoration;

	private static final Image ICON_WARNING;
	
	/** list of features this decorator watches for changes */
	private static EList<EStructuralFeature> subscribedFeatures;

	static {
		/*
		 * prefix path with "$nl$" and use Plugin.find() to search for the
		 * locale specific file
		 */
		IPath path = new Path("$nl$").append( //$NON-NLS-1$
				"icons//full//obj16//warn_tsk.gif"); //$NON-NLS-1$
        URL url = FileLocator.find(hub.top.editor.Activator.getDefault().getBundle(), path, null);
        ImageDescriptor imgDesc = ImageDescriptor.createFromURL(url);
		ICON_WARNING = imgDesc.createImage();
		
		// initialize list of features to watch for
		subscribedFeatures = new BasicEList<EStructuralFeature>();
		subscribedFeatures.add(PtnetLoLAPackage.eINSTANCE.getNode_Name());
	}

	/**
	 * Creates a new <code>AbstractDecorator</code> for the given 
	 * decoratorTarget
	 * 
	 * @param decoratorTarget  the object to be decorated
	 */
	public NodeDecorator(IDecoratorTarget decoratorTarget) {
		this.decoratorTarget = decoratorTarget;
	}

	/**
	 * @return Returns the object to be decorated
	 */
	protected IDecoratorTarget getDecoratorTarget() {
		return decoratorTarget;
	}

	public IDecoration getDecoration() {
		return decoration;
	}

	public void setDecoration(IDecoration decoration) {
		this.decoration = decoration;
	}

	/**
	 * Removes the decoration if it exists and sets it to null.
	 */
	protected void removeDecoration() {
		if (decoration != null) {
			decoratorTarget.removeDecoration(decoration);
			decoration = null;
		}
	}

	/**
	 * Retrieve the node that corresponds to the given decoratorTarget
	 * and that can be decorated with this decorator. 
	 * 
	 * @param decoratorTarget  IDecoratorTarget to check
	 * @return node Node if IDecoratorTarget can be supported, null otherwise
	 */
	static public Node getDecoratorTargetNode(IDecoratorTarget decoratorTarget) {
		
		View node = (View) decoratorTarget.getAdapter(View.class);
		if (node != null && node.eContainer() instanceof Diagram) {
			if (node.getElement() instanceof hub.top.editor.ptnetLoLA.Node)
				return (Node)node;
		}
		return null;
	}


	/**
	 * Creates the appropriate review decoration if all the criteria is
	 * satisfied by the view passed in.
	 */
	public void refresh() {
		removeDecoration();

		Node node = getDecoratorTargetNode(getDecoratorTarget());

		if (node != null) {
			boolean failed = false;
			boolean applyDecorator = false;
		
			if (node.getElement() instanceof hub.top.editor.ptnetLoLA.Node) {
				applyDecorator = true;
				
				hub.top.editor.ptnetLoLA.Node n = (hub.top.editor.ptnetLoLA.Node)node.getElement();
				failed = !PtnetLoLAValidator.validate(n);
			}
			
			if (applyDecorator) {
				if (failed) {
					setDecoration(getDecoratorTarget().addShapeDecoration(ICON_WARNING, IDecoratorTarget.Direction.EAST, 0, false));
				}
			}
		}
	}

	private NotificationListener notificationListener = new NotificationListener() {

        /* (non-Javadoc)
         * @see org.eclipse.gmf.runtime.diagram.core.listener.NotificationListener#notifyChanged(org.eclipse.emf.common.notify.Notification)
         */
        public void notifyChanged(Notification notification) {
            refresh();
        }
	};
	
	
	
	/**
	 * Adds decoration if applicable.
	 */
	public void activate() {
        
        IGraphicalEditPart gep = (IGraphicalEditPart)getDecoratorTarget().getAdapter(IGraphicalEditPart.class);
        assert gep != null;

        DiagramEventBroker deb = DiagramEventBroker.getInstance(gep.getEditingDomain());
        
        // subscribe the decorator to all features that may cause changes
        for (EStructuralFeature feat : subscribedFeatures) {
        	EObject modelObject = gep.getPrimaryView().getElement();
        	
        	// see if the current object can be watched for the current feature
        	if (!modelObject.eClass().getEAllStructuralFeatures().contains(feat))
        		continue;

    		// yes, listen on the model element for the current feature
        	deb.addNotificationListener(modelObject, feat, notificationListener);
        }
	}

	/**
	 * Removes the decoration.
	 */
	public void deactivate() {
		removeDecoration();

        IGraphicalEditPart gep = (IGraphicalEditPart)getDecoratorTarget().getAdapter(IGraphicalEditPart.class);
        assert gep != null;
        
        DiagramEventBroker deb = DiagramEventBroker.getInstance(gep.getEditingDomain());
        
        // subscribe the decorator to all features that may cause changes
        for (EStructuralFeature feat : subscribedFeatures) {
        	EObject modelObject = gep.getPrimaryView().getElement();
        	
        	// see if the current object can be watched for the current feature
        	if (!modelObject.eClass().getEAllStructuralFeatures().contains(feat))
        		continue;

    		// yes, listen on the model element for the current feature
        	deb.removeNotificationListener(modelObject, feat, notificationListener);
        }
	}
}
