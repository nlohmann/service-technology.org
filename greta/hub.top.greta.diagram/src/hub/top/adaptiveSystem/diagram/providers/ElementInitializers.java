package hub.top.adaptiveSystem.diagram.providers;

import hub.top.adaptiveSystem.AdaptiveSystemPackage;

import hub.top.adaptiveSystem.diagram.expressions.AdaptiveSystemAbstractExpression;
import hub.top.adaptiveSystem.diagram.expressions.AdaptiveSystemOCLFactory;

import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;

/**
 * @generated
 */
public class ElementInitializers {

	/**
	 * @generated
	 */
	public static class Initializers {
		/**
		 * @generated
		 */
		public static final IObjectInitializer Oclet_1002 = new ObjectInitializer(
				hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
						.getOclet()) {
			protected void init() {
				add(createNewElementFeatureInitializer(
						AdaptiveSystemPackage.eINSTANCE.getOclet_PreNet(),
						new ObjectInitializer[] { preNet(), }));
				add(createNewElementFeatureInitializer(
						AdaptiveSystemPackage.eINSTANCE.getOclet_DoNet(),
						new ObjectInitializer[] { doNet(), }));
			}

			ObjectInitializer preNet() {
				return new ObjectInitializer(
						hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
								.getPreNet()) {
					protected void init() {
						add(createExpressionFeatureInitializer(
								AdaptiveSystemPackage.eINSTANCE
										.getOccurrenceNet_Name(),
								AdaptiveSystemOCLFactory.getExpression(
										"\'preNet\'", //$NON-NLS-1$
										AdaptiveSystemPackage.eINSTANCE
												.getPreNet())));
					}
				}; // preNet ObjectInitializer
			}

			ObjectInitializer doNet() {
				return new ObjectInitializer(
						hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
								.getDoNet()) {
					protected void init() {
						add(createExpressionFeatureInitializer(
								AdaptiveSystemPackage.eINSTANCE
										.getOccurrenceNet_Name(),
								AdaptiveSystemOCLFactory.getExpression(
										"\'doNet\'", //$NON-NLS-1$
										AdaptiveSystemPackage.eINSTANCE
												.getDoNet())));
					}
				}; // doNet ObjectInitializer
			}
		}; // Oclet_1002 ObjectInitializer

		/** 
		 * @generated
		 */
		private Initializers() {
		}

		/** 
		 * @generated
		 */
		public static interface IObjectInitializer {
			/** 
			 * @generated
			 */
			public void init(EObject instance);
		}

		/** 
		 * @generated
		 */
		public static abstract class ObjectInitializer implements
				IObjectInitializer {
			/** 
			 * @generated
			 */
			final EClass element;
			/** 
			 * @generated
			 */
			private List featureInitializers = new ArrayList();

			/** 
			 * @generated
			 */
			ObjectInitializer(EClass element) {
				this.element = element;
				init();
			}

			/**
			 * @generated
			 */
			protected abstract void init();

			/** 
			 * @generated
			 */
			protected final IFeatureInitializer add(
					IFeatureInitializer initializer) {
				featureInitializers.add(initializer);
				return initializer;
			}

			/** 
			 * @generated
			 */
			public void init(EObject instance) {
				for (Iterator it = featureInitializers.iterator(); it.hasNext();) {
					IFeatureInitializer nextExpr = (IFeatureInitializer) it
							.next();
					try {
						nextExpr.init(instance);
					} catch (RuntimeException e) {
						AdaptiveSystemDiagramEditorPlugin.getInstance()
								.logError("Feature initialization failed", e); //$NON-NLS-1$						
					}
				}
			}
		} // end of ObjectInitializer

		/** 
		 * @generated
		 */
		interface IFeatureInitializer {
			/**
			 * @generated
			 */
			void init(EObject contextInstance);
		}

		/**
		 * @generated
		 */
		static IFeatureInitializer createNewElementFeatureInitializer(
				EStructuralFeature initFeature,
				ObjectInitializer[] newObjectInitializers) {
			final EStructuralFeature feature = initFeature;
			final ObjectInitializer[] initializers = newObjectInitializers;
			return new IFeatureInitializer() {
				public void init(EObject contextInstance) {
					for (int i = 0; i < initializers.length; i++) {
						EObject newInstance = initializers[i].element
								.getEPackage().getEFactoryInstance().create(
										initializers[i].element);
						if (feature.isMany()) {
							((Collection) contextInstance.eGet(feature))
									.add(newInstance);
						} else {
							contextInstance.eSet(feature, newInstance);
						}
						initializers[i].init(newInstance);
					}
				}
			};
		}

		/**
		 * @generated
		 */
		static IFeatureInitializer createExpressionFeatureInitializer(
				EStructuralFeature initFeature,
				AdaptiveSystemAbstractExpression valueExpression) {
			final EStructuralFeature feature = initFeature;
			final AdaptiveSystemAbstractExpression expression = valueExpression;
			return new IFeatureInitializer() {
				public void init(EObject contextInstance) {
					expression.assignTo(feature, contextInstance);
				}
			};
		}
	} // end of Initializers
}
