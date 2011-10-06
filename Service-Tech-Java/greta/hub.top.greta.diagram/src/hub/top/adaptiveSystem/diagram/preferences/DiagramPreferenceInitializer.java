package hub.top.adaptiveSystem.diagram.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

/**
 * @generated
 */
public class DiagramPreferenceInitializer extends AbstractPreferenceInitializer {

	/**
	 * @generated
	 */
	public void initializeDefaultPreferences() {
		IPreferenceStore store = getPreferenceStore();
		hub.top.adaptiveSystem.diagram.preferences.DiagramPrintingPreferencePage
				.initDefaults(store);
		hub.top.adaptiveSystem.diagram.preferences.DiagramGeneralPreferencePage
				.initDefaults(store);
		hub.top.adaptiveSystem.diagram.preferences.DiagramAppearancePreferencePage
				.initDefaults(store);
		hub.top.adaptiveSystem.diagram.preferences.DiagramConnectionsPreferencePage
				.initDefaults(store);
		hub.top.adaptiveSystem.diagram.preferences.DiagramRulersAndGridPreferencePage
				.initDefaults(store);
	}

	/**
	 * @generated
	 */
	protected IPreferenceStore getPreferenceStore() {
		return hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
				.getInstance().getPreferenceStore();
	}
}
