/*****************************************************************************\
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
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
 * The Original Code is this file as it was released on November 26, 2008.
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

package hub.top.adaptiveSystem.gmf;

import hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Border;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.handles.NonResizableHandleKit;
import org.eclipse.gef.handles.ResizableHandleKit;
import org.eclipse.gmf.runtime.diagram.ui.commands.ICommandProxy;
import org.eclipse.gmf.runtime.diagram.ui.commands.SetBoundsCommand;
import org.eclipse.gmf.runtime.diagram.ui.editparts.GraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.l10n.DiagramUIMessages;
import org.eclipse.swt.SWT;

public class OcletResizableEditPolicy extends ResizableEditPolicy {
	
	private GraphicalEditPart fShape;
	
	private int layoutDirections;
	
	public OcletResizableEditPolicy (GraphicalEditPart part, int resizeDirections, int layoutDirections) {
		super();

		setDragAllowed(false);
		setResizeDirections(resizeDirections);

		this.fShape = part;
		this.layoutDirections = layoutDirections;
	}
	
	/**
	 * @see org.eclipse.gef.editpolicies.SelectionHandlesEditPolicy#createSelectionHandles()
	 */
	@SuppressWarnings("unchecked")
	protected List createSelectionHandles() {
		List list = new ArrayList();
		
		int directions = getResizeDirections();
		
		if (directions == 0 && isDragAllowed())
			NonResizableHandleKit.addHandles((GraphicalEditPart)getHost(), list);
		else if (directions != -1) {
			ResizableHandleKit.addMoveHandle((GraphicalEditPart)getHost(), list);
			if ((directions & PositionConstants.EAST) != 0)
				ResizableHandleKit.addHandle((GraphicalEditPart)getHost(), list, 
						PositionConstants.EAST);
			if ((directions & PositionConstants.SOUTH_EAST) == PositionConstants.SOUTH_EAST)
				ResizableHandleKit.addHandle((GraphicalEditPart)getHost(), list, 
						PositionConstants.SOUTH_EAST);
			if ((directions & PositionConstants.SOUTH) != 0)
				ResizableHandleKit.addHandle((GraphicalEditPart)getHost(), list, 
						PositionConstants.SOUTH);
			if ((directions & PositionConstants.SOUTH_WEST) == PositionConstants.SOUTH_WEST)
				ResizableHandleKit.addHandle((GraphicalEditPart)getHost(), list, 
						PositionConstants.SOUTH_WEST);
			if ((directions & PositionConstants.WEST) != 0)
				ResizableHandleKit.addHandle((GraphicalEditPart)getHost(), list, 
						PositionConstants.WEST);
			if ((directions & PositionConstants.NORTH_WEST) == PositionConstants.NORTH_WEST)
				ResizableHandleKit.addHandle((GraphicalEditPart)getHost(), list, 
						PositionConstants.NORTH_WEST);
			if ((directions & PositionConstants.NORTH) != 0)
				ResizableHandleKit.addHandle((GraphicalEditPart)getHost(), list, 
						PositionConstants.NORTH);
			if ((directions & PositionConstants.NORTH_EAST) == PositionConstants.NORTH_EAST)
				ResizableHandleKit.addHandle((GraphicalEditPart)getHost(), list, 
						PositionConstants.NORTH_EAST);
		}
		
	 	return list;
	}

	/**
	 * @see org.eclipse.gef.editpolicies.ResizableEditPolicy#getCommand(org.eclipse.gef.Request)
	 */
	@Override
	public Command getCommand(Request request) {
		CompoundCommand cc = new CompoundCommand();
		if (request.getType() == AutoResizePredictRequest.REQ_AUTO_RESIZE_PREDICT) {
			AutoResizePredictRequest arp = (AutoResizePredictRequest)request;
			
			OcletEditPart oep = (OcletEditPart)fShape.getParent();
			
			if (arp.getResizeTrigger() == oep.getContentPane()) {
				TransactionalEditingDomain editingDomain = fShape.getEditingDomain();
	
				int width = fShape.getFigure().getBounds().width;
				int height = fShape.getFigure().getBounds().height;
				int x = fShape.getFigure().getBounds().x;
				int y = fShape.getFigure().getBounds().y;

				// position relative to parent shape
				int x_rel = x - oep.getFigure().getBounds().x;
				int y_rel = y - oep.getFigure().getBounds().y;
				
				int borderOffset = 0;
				Border border = oep.getPrimaryShape().getBorder();
				if (border != null && border instanceof LineBorder) {
					borderOffset = ((LineBorder)border).getWidth();
				}
				
				if ((layoutDirections & SWT.VERTICAL) != 0) {
					// bounds to the left
					x = 0;
					// and bounds to the right
					width = arp.getPredictedSize().width - 2*borderOffset;
					
					if ((layoutDirections & SWT.CENTER) != 0) {
						// set top of preNet under the header
						y = oep.getPrimaryShape().getFigureOcletHeader().getBounds().height;
						// height of the preNet is a free parameter
					} else if ((layoutDirections & SWT.BOTTOM) != 0) {
						// set height of doNet relative to preNet and oclet size
						height = arp.getPredictedSize().height - y_rel - borderOffset;
						
						// set top of doNet directly under the preNet
						Rectangle preNetBounds = oep.getPreNetEditPart().getFigure().getBounds(); 
						y = preNetBounds.height	+ (preNetBounds.y - oep.getFigure().getBounds().y - borderOffset);
					}
				} else {
					// unknown layout, keep x, y, width and height
					x = x_rel;
					y = y_rel;
				}
	
				// create command to reposition the shape
				Point p = new Point(x, y);
				Dimension d = new Dimension(width, height);
				
				SetBoundsCommand c = new SetBoundsCommand(editingDomain,
						DiagramUIMessages.SetAutoSizeCommand_Label,
						fShape, new Rectangle(p,d));
	
				cc.add(new ICommandProxy(c));
			}
		}
		
		Command c2 = super.getCommand(request);
		if (c2 != null)	cc.add(c2);
		
		if (cc.size() > 0) {
			cc.unwrap();
			return cc;
		}
		else
			return null;
	}

	/**
	 * @see org.eclipse.gef.editpolicies.ResizableEditPolicy#understandsRequest(org.eclipse.gef.Request)
	 */
	@Override
	public boolean understandsRequest(Request req) {
		if (req.getType() == AutoResizePredictRequest.REQ_AUTO_RESIZE_PREDICT)
			return true;
		else 
			return super.understandsRequest(req);
	}
}
