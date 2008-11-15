package hub.top.adaptiveSystem.diagram.providers;

/**
 * @generated
 */
public class ElementInitializers {
	/**
	 * @generated
	 */
	public static void init_Oclet_1002(hub.top.adaptiveSystem.Oclet instance) {
		try {
			hub.top.adaptiveSystem.PreNet newInstance_0_0 = hub.top.adaptiveSystem.AdaptiveSystemFactory.eINSTANCE
					.createPreNet();

			instance.setPreNet(newInstance_0_0);
			Object value_0_0_0 = hub.top.adaptiveSystem.diagram.expressions.AdaptiveSystemOCLFactory
					.getExpression(
							"preNet",
							hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
									.getPreNet()).evaluate(newInstance_0_0);
			newInstance_0_0.setName((String) value_0_0_0);

			hub.top.adaptiveSystem.DoNet newInstance_1_0 = hub.top.adaptiveSystem.AdaptiveSystemFactory.eINSTANCE
					.createDoNet();

			instance.setDoNet(newInstance_1_0);
			Object value_1_0_0 = hub.top.adaptiveSystem.diagram.expressions.AdaptiveSystemOCLFactory
					.getExpression(
							"doNet",
							hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
									.getDoNet()).evaluate(newInstance_1_0);
			newInstance_1_0.setName((String) value_1_0_0);

		} catch (RuntimeException e) {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError("Element initialization failed", e); //$NON-NLS-1$						
		}
	}

}
