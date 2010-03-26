/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *  		Dirk Fahland
 *  
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
 */
package hub.top.lang.flowcharts.diagram.part;

import hub.top.lang.flowcharts.provider.FlowItemProviderAdapterFactory;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.edit.provider.ComposedAdapterFactory;
import org.eclipse.emf.edit.provider.IItemLabelProvider;
import org.eclipse.emf.edit.provider.ReflectiveItemProviderAdapterFactory;
import org.eclipse.emf.edit.provider.resource.ResourceItemProviderAdapterFactory;
import org.eclipse.emf.edit.ui.provider.ExtendedImageRegistry;
import org.eclipse.gmf.runtime.diagram.core.preferences.PreferencesHint;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

/**
 * @generated
 */
public class FlowchartDiagramEditorPlugin extends AbstractUIPlugin {

  /**
   * @generated
   */
  public static final String ID = "hub.top.lang.flowcharts.diagram"; //$NON-NLS-1$

  /**
   * @generated
   */
  public static final PreferencesHint DIAGRAM_PREFERENCES_HINT = new PreferencesHint(
      ID);

  /**
   * @generated
   */
  private static FlowchartDiagramEditorPlugin instance;

  /**
   * @generated
   */
  private ComposedAdapterFactory adapterFactory;

  /**
   * @generated
   */
  private FlowchartDocumentProvider documentProvider;

  /**
   * @generated
   */
  public FlowchartDiagramEditorPlugin() {
  }

  /**
   * @generated
   */
  public void start(BundleContext context) throws Exception {
    super.start(context);
    instance = this;
    PreferencesHint.registerPreferenceStore(DIAGRAM_PREFERENCES_HINT,
        getPreferenceStore());
    adapterFactory = createAdapterFactory();
  }

  /**
   * @generated
   */
  public void stop(BundleContext context) throws Exception {
    adapterFactory.dispose();
    adapterFactory = null;
    instance = null;
    super.stop(context);
  }

  /**
   * @generated
   */
  public static FlowchartDiagramEditorPlugin getInstance() {
    return instance;
  }

  /**
   * @generated
   */
  protected ComposedAdapterFactory createAdapterFactory() {
    List factories = new ArrayList();
    fillItemProviderFactories(factories);
    return new ComposedAdapterFactory(factories);
  }

  /**
   * @generated
   */
  protected void fillItemProviderFactories(List factories) {
    factories.add(new FlowItemProviderAdapterFactory());
    factories.add(new ResourceItemProviderAdapterFactory());
    factories.add(new ReflectiveItemProviderAdapterFactory());
  }

  /**
   * @generated
   */
  public AdapterFactory getItemProvidersAdapterFactory() {
    return adapterFactory;
  }

  /**
   * @generated
   */
  public ImageDescriptor getItemImageDescriptor(Object item) {
    IItemLabelProvider labelProvider = (IItemLabelProvider) adapterFactory
        .adapt(item, IItemLabelProvider.class);
    if (labelProvider != null) {
      return ExtendedImageRegistry.getInstance().getImageDescriptor(
          labelProvider.getImage(item));
    }
    return null;
  }

  /**
   * Returns an image descriptor for the image file at the given
   * plug-in relative path.
   *
   * @generated
   * @param path the path
   * @return the image descriptor
   */
  public static ImageDescriptor getBundledImageDescriptor(String path) {
    return AbstractUIPlugin.imageDescriptorFromPlugin(ID, path);
  }

  /**
   * Respects images residing in any plug-in. If path is relative,
   * then this bundle is looked up for the image, otherwise, for absolute 
   * path, first segment is taken as id of plug-in with image
   *
   * @generated
   * @param path the path to image, either absolute (with plug-in id as first segment), or relative for bundled images
   * @return the image descriptor
   */
  public static ImageDescriptor findImageDescriptor(String path) {
    final IPath p = new Path(path);
    if (p.isAbsolute() && p.segmentCount() > 1) {
      return AbstractUIPlugin.imageDescriptorFromPlugin(p.segment(0), p
          .removeFirstSegments(1).makeAbsolute().toString());
    } else {
      return getBundledImageDescriptor(p.makeAbsolute().toString());
    }
  }

  /**
   * Returns an image for the image file at the given plug-in relative path.
   * Client do not need to dispose this image. Images will be disposed automatically.
   *
   * @generated
   * @param path the path
   * @return image instance
   */
  public Image getBundledImage(String path) {
    Image image = getImageRegistry().get(path);
    if (image == null) {
      getImageRegistry().put(path, getBundledImageDescriptor(path));
      image = getImageRegistry().get(path);
    }
    return image;
  }

  /**
   * Returns string from plug-in's resource bundle
   *
   * @generated
   */
  public static String getString(String key) {
    return Platform.getResourceString(getInstance().getBundle(), "%" + key); //$NON-NLS-1$
  }

  /**
   * @generated
   */
  public FlowchartDocumentProvider getDocumentProvider() {
    if (documentProvider == null) {
      documentProvider = new FlowchartDocumentProvider();
    }
    return documentProvider;
  }

  /**
   * @generated
   */
  public void logError(String error) {
    logError(error, null);
  }

  /**
   * @generated
   */
  public void logError(String error, Throwable throwable) {
    if (error == null && throwable != null) {
      error = throwable.getMessage();
    }
    getLog().log(
        new Status(IStatus.ERROR, FlowchartDiagramEditorPlugin.ID, IStatus.OK,
            error, throwable));
    debug(error, throwable);
  }

  /**
   * @generated
   */
  public void logInfo(String message) {
    logInfo(message, null);
  }

  /**
   * @generated
   */
  public void logInfo(String message, Throwable throwable) {
    if (message == null && throwable != null) {
      message = throwable.getMessage();
    }
    getLog().log(
        new Status(IStatus.INFO, FlowchartDiagramEditorPlugin.ID, IStatus.OK,
            message, throwable));
    debug(message, throwable);
  }

  /**
   * @generated
   */
  private void debug(String message, Throwable throwable) {
    if (!isDebugging()) {
      return;
    }
    if (message != null) {
      System.err.println(message);
    }
    if (throwable != null) {
      throwable.printStackTrace();
    }
  }
}
