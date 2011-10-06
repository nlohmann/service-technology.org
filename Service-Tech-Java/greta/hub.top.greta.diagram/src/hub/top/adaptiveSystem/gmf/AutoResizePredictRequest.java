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

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.gef.Request;

/**
 * A request that asks all subscribes to automatically resize. The request
 * carries data about the trigger of the request. The request is sent in
 * cases where the triggering object is about to change its size as well,
 * but has not changed yet. To indicate the change, the request provides the
 * predicted size of the triggering object.
 * 
 * @author Dirk Fahland
 */
public class AutoResizePredictRequest extends Request {

	public static final String REQ_AUTO_RESIZE_PREDICT = "predictive auto-resize"; 

	private IFigure resizeTrigger;
	private Dimension predictedSize;
	
	/**
	 * a new auto-resize request that is triggered by the <code>resizeTrigger</code>
	 * that is predicted to have bounds of <code>newSize</code>
	 * 
	 * @param resizeTrigger
	 * @param newSize
	 */
	public AutoResizePredictRequest (IFigure resizeTrigger, Dimension newSize) {
		super(REQ_AUTO_RESIZE_PREDICT);
		
		this.resizeTrigger = resizeTrigger;
		this.predictedSize = newSize;
	}
	
	/**
	 * @return the predicted size of the trigger of this resize request
	 */
	public Dimension getPredictedSize () {
		return predictedSize;
	}
	
	/**
	 * @return the edit part that has triggered this resize request
	 */
	public IFigure getResizeTrigger () {
		return resizeTrigger;
	}
}
