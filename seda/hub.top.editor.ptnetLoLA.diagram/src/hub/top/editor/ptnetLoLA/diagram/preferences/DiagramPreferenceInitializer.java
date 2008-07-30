package hub.top.editor.ptnetLoLA.diagram.preferences;

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
		hub.top.editor.ptnetLoLA.diagram.preferences.DiagramPrintingPreferencePage
				.initDefaults(store);
		hub.top.editor.ptnetLoLA.diagram.preferences.DiagramGeneralPreferencePage
				.initDefaults(store);
		hub.top.editor.ptnetLoLA.diagram.preferences.DiagramAppearancePreferencePage
				.initDefaults(store);
		hub.top.editor.ptnetLoLA.diagram.preferences.DiagramConnectionsPreferencePage
				.initDefaults(store);
		hub.top.editor.ptnetLoLA.diagram.preferences.DiagramRulersAndGridPreferencePage
				.initDefaults(store);
	}

	/**
	 * @generated
	 */
	protected IPreferenceStore getPreferenceStore() {
		return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
				.getInstance().getPreferenceStore();
	}
}
