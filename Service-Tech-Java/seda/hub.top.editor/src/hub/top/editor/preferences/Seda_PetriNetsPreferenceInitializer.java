package hub.top.editor.preferences;

import hub.top.editor.Activator;

import org.eclipse.core.runtime.preferences.AbstractPreferenceInitializer;
import org.eclipse.jface.preference.IPreferenceStore;

public class Seda_PetriNetsPreferenceInitializer extends AbstractPreferenceInitializer {

  public void initializeDefaultPreferences() {
    IPreferenceStore store = getPreferenceStore();
    hub.top.editor.preferences.Seda_PetriNetsPreferencePage.initDefaults(store);

  }

  protected IPreferenceStore getPreferenceStore() {
    return Activator.getDefault().getPreferenceStore();
  }
  

}
