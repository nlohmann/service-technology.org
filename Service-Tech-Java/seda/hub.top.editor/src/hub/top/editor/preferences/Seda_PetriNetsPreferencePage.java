package hub.top.editor.preferences;

import org.eclipse.jface.preference.*;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.eclipse.ui.IWorkbench;
import hub.top.editor.Activator;

/**
 * A preference page for the nitpicker GUI
 */

public class Seda_PetriNetsPreferencePage
	extends FieldEditorPreferencePage
	implements IWorkbenchPreferencePage {

	public Seda_PetriNetsPreferencePage() {
		super(GRID);
		setPreferenceStore(Activator.getDefault().getPreferenceStore());
		setDescription("Petri Nets (provided by Seda)");
	}
	
	/**
	 * Create the fields of the preference page by setting corresponding
	 * editors.
	 */
	@Override
	public void createFieldEditors() {
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.IWorkbenchPreferencePage#init(org.eclipse.ui.IWorkbench)
	 */
	public void init(IWorkbench workbench) {
	}
	
  /**
   * Initializes the default preference values for the preferences.
   * 
   * @param IPreferenceStore preferences
   */
  public static void initDefaults(IPreferenceStore preferenceStore) {
    //preferenceStore.setDefault(key, value);
  }
	
}