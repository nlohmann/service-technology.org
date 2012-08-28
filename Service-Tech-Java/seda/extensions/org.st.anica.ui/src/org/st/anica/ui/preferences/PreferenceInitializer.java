package org.st.anica.ui.preferences;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;
import org.st.anica.ui.Activator;

/**
 * Class used to initialize default preference values.
 */
public class PreferenceInitializer extends AbstractPreferenceInitializer {

	/*
	 * (non-Javadoc)
	 * 
	 * @see org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer#initializeDefaultPreferences()
	 */
	public void initializeDefaultPreferences() {
		IPreferenceStore store = Activator.getDefault().getPreferenceStore();
		store.setDefault(PreferenceConstants.P_ANICA_SERVER_URL, "localhost");
		store.setDefault(PreferenceConstants.P_ANICA_SERVER_PORT, "5556");
		store.setDefault(PreferenceConstants.P_ANICA_LOCAL_PORT, "5555");
		
		store.setDefault(PreferenceConstants.P_ANICA_PACKETSIZE, 64);
		store.setDefault(PreferenceConstants.P_ANICA_TIMEOUT, 5000);
	}
}
