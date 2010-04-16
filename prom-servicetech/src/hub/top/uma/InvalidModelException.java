/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. AGPL3.0
 * 
 * ServiceTechnology.org - Uma, an Unfolding-based Model Analyzer
 * 
 * This program and the accompanying materials are made available under
 * the terms of the GNU Affero General Public License Version 3 or later,
 * which accompanies this distribution, and is available at 
 * http://www.gnu.org/licenses/agpl.txt
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
\*****************************************************************************/

package hub.top.uma;

/**
 * Expresses that the input model for the verification is invalid.
 *  
 * @author Dirk Fahland
 */
public class InvalidModelException extends Exception {

  /**
   * 
   */
  private static final long serialVersionUID = 8091384676313192979L;

  public static final int EMPTY_PRESET = 1;
  
  private Object cause;
  private int reason;
  
  public InvalidModelException(int reason, Object e, String text) {
    super(text);
    this.reason = reason;
    this.cause = e;
  }
  
  public Object getCausingObject() {
    return cause;
  }
  
  public int getReason() {
    return reason;
  }
}
