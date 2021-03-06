/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.validation;

import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.WorkflowDiagram;

import org.eclipse.emf.common.util.EList;

/**
 * A sample validator interface for {@link hub.top.lang.flowcharts.ResourceNode}.
 * This doesn't really do anything, and it's not a real EMF artifact.
 * It was generated by the org.eclipse.emf.examples.generator.validator plug-in to illustrate how EMF's code generator can be extended.
 * This can be disabled with -vmargs -Dorg.eclipse.emf.examples.generator.validator=false.
 */
public interface ResourceNodeValidator {
	boolean validate();

	boolean validateActivityInput(EList<ActivityNode> value);
	boolean validateActivityInputOpt(EList<ActivityNode> value);
	boolean validateProcessInputOpt(EList<WorkflowDiagram> value);
	boolean validateProcessInput(EList<WorkflowDiagram> value);
}
