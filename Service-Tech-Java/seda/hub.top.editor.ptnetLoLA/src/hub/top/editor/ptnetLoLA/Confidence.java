/**
 *  Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 *  
 *  ServiceTechnolog.org - PetriNet Editor Framework
 *  
 *  This program and the accompanying materials are made available under
 *  the terms of the Eclipse Public License v1.0, which accompanies this
 *  distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 *  
 *  Software distributed under the License is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *  for the specific language governing rights and limitations under the
 *  License.
 * 
 *  The Original Code is this file as it was released on July 30, 2008.
 *  The Initial Developer of the Original Code is
 *  		Dirk Fahland
 *  
 *  Portions created by the Initial Developer are Copyright (c) 2008
 *  the Initial Developer. All Rights Reserved.
 * 
 *  Contributor(s):
 * 
 *  Alternatively, the contents of this file may be used under the terms of
 *  either the GNU General Public License Version 3 or later (the "GPL"), or
 *  the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 *  in which case the provisions of the GPL or the LGPL are applicable instead
 *  of those above. If you wish to allow use of your version of this file only
 *  under the terms of either the GPL or the LGPL, and not to allow others to
 *  use your version of this file under the terms of the EPL, indicate your
 *  decision by deleting the provisions above and replace them with the notice
 *  and other provisions required by the GPL or the LGPL. If you do not delete
 *  the provisions above, a recipient may use your version of this file under
 *  the terms of any one of the EPL, the GPL or the LGPL.
 */
package hub.top.editor.ptnetLoLA;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.eclipse.emf.common.util.Enumerator;

/**
 * <!-- begin-user-doc -->
 * A representation of the literals of the enumeration '<em><b>Confidence</b></em>',
 * and utility methods for working with them.
 * <!-- end-user-doc -->
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getConfidence()
 * @model
 * @generated
 */
public enum Confidence implements Enumerator {
  /**
   * The '<em><b>UNKNOWN</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #UNKNOWN_VALUE
   * @generated
   * @ordered
   */
  UNKNOWN(0, "UNKNOWN", ""), /**
   * The '<em><b>LOW</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #LOW_VALUE
   * @generated
   * @ordered
   */
  LOW(1, "LOW", "LOW"), /**
   * The '<em><b>HIGH</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #HIGH_VALUE
   * @generated
   * @ordered
   */
  HIGH(2, "HIGH", "HIGH");

  /**
   * The '<em><b>UNKNOWN</b></em>' literal value.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of '<em><b>UNKNOWN</b></em>' literal object isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @see #UNKNOWN
   * @model literal=""
   * @generated
   * @ordered
   */
  public static final int UNKNOWN_VALUE = 0;

  /**
   * The '<em><b>LOW</b></em>' literal value.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of '<em><b>LOW</b></em>' literal object isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @see #LOW
   * @model
   * @generated
   * @ordered
   */
  public static final int LOW_VALUE = 1;

  /**
   * The '<em><b>HIGH</b></em>' literal value.
   * <!-- begin-user-doc -->
   * <p>
   * If the meaning of '<em><b>HIGH</b></em>' literal object isn't clear,
   * there really should be more of a description here...
   * </p>
   * <!-- end-user-doc -->
   * @see #HIGH
   * @model
   * @generated
   * @ordered
   */
  public static final int HIGH_VALUE = 2;

  /**
   * An array of all the '<em><b>Confidence</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private static final Confidence[] VALUES_ARRAY =
    new Confidence[] {
      UNKNOWN,
      LOW,
      HIGH,
    };

  /**
   * A public read-only list of all the '<em><b>Confidence</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public static final List<Confidence> VALUES = Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Confidence</b></em>' literal with the specified literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public static Confidence get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      Confidence result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Confidence</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public static Confidence getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      Confidence result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Confidence</b></em>' literal with the specified integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public static Confidence get(int value) {
    switch (value) {
      case UNKNOWN_VALUE: return UNKNOWN;
      case LOW_VALUE: return LOW;
      case HIGH_VALUE: return HIGH;
    }
    return null;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private final int value;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private final String name;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private final String literal;

  /**
   * Only this class can construct instances.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  private Confidence(int value, String name, String literal) {
    this.value = value;
    this.name = name;
    this.literal = literal;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public int getValue() {
    return value;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getName() {
    return name;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  public String getLiteral() {
    return literal;
  }

  /**
   * Returns the literal value of the enumerator, which is its string representation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public String toString() {
    return literal;
  }
  
} //Confidence
