package hub.top.editor.ptnetLoLA.diagram.edit.policies;

import org.eclipse.gef.commands.Command;
import org.eclipse.gmf.runtime.emf.type.core.commands.DestroyElementCommand;
import org.eclipse.gmf.runtime.emf.type.core.requests.DestroyElementRequest;

/**
 * @generated
 */
public class ArcToTransitionItemSemanticEditPolicy
    extends
    hub.top.editor.ptnetLoLA.diagram.edit.policies.PtnetLoLABaseItemSemanticEditPolicy {

  /**
   * @generated
   */
  public ArcToTransitionItemSemanticEditPolicy() {
    super(
        hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002);
  }

  /**
   * @generated
   */
  protected Command getDestroyElementCommand(DestroyElementRequest req) {
    return getGEFWrapper(new DestroyElementCommand(req));
  }

}
