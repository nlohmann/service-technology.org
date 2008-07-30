package hub.top.editor.ptnetLoLA.diagram.expressions;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.Map;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.emf.ecore.EClassifier;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.ETypedElement;
import org.eclipse.emf.ecore.util.EcoreUtil;

/**
 * @generated
 */
public abstract class PtnetLoLAAbstractExpression {

	/**
	 * @generated
	 */
	private static final boolean DISABLED_NO_IMPL_EXCEPTION_LOG = Boolean
			.valueOf(
					Platform
							.getDebugOption(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
									.getInstance().getBundle()
									.getSymbolicName()
									+ "/debug/disableNoExprImplExceptionLog"))
			.booleanValue();

	/**
	 * @generated
	 */
	private final String body;

	/**
	 * @generated
	 */
	private final EClassifier context;

	/**
	 * @generated
	 */
	private IStatus status = Status.OK_STATUS;

	/**
	 * @generated
	 */
	protected PtnetLoLAAbstractExpression(EClassifier context) {
		this(null, context);
	}

	/**
	 * @generated
	 */
	protected PtnetLoLAAbstractExpression(String body, EClassifier context) {
		this.body = body;
		this.context = context;
	}

	/**
	 * @generated
	 */
	protected void setStatus(int severity, String message, Throwable throwable) {
		String pluginID = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin.ID;
		this.status = new Status(severity, pluginID, -1,
				(message != null) ? message : "", throwable); //$NON-NLS-1$
		if (!this.status.isOK()) {
			hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
					.getInstance()
					.logError(
							"Expression problem: " + message + " body: " + body, throwable); //$NON-NLS-1$ //$NON-NLS-2$
		}
	}

	/**
	 * @generated
	 */
	protected abstract Object doEvaluate(Object context, Map env);

	/**
	 * @generated
	 */
	public Object evaluate(Object context) {
		return evaluate(context, Collections.EMPTY_MAP);
	}

	/**
	 * @generated
	 */
	public Object evaluate(Object context, Map env) {
		if (context().isInstance(context)) {
			try {
				return doEvaluate(context, env);
			} catch (Exception e) {
				if (DISABLED_NO_IMPL_EXCEPTION_LOG
						&& e instanceof NoImplException) {
					return null;
				}
				hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
						.getInstance().logError(
								"Expression evaluation failure: " + body, e);
			}
		}
		return null;
	}

	/**
	 * @generated
	 */
	public IStatus getStatus() {
		return status;
	}

	/**
	 * @generated
	 */
	public String body() {
		return body;
	}

	/**
	 * @generated
	 */
	public EClassifier context() {
		return context;
	}

	/**
	 * @generated
	 */
	public void assignTo(EStructuralFeature feature, EObject target) {
		Object value = evaluate(target);
		value = (value != null) ? performCast(value, feature) : null;
		if (feature.isMany()) {
			Collection destCollection = (Collection) target.eGet(feature);
			destCollection.clear();
			if (value instanceof Collection) {
				Collection valueCollection = (Collection) value;
				for (Iterator it = valueCollection.iterator(); it.hasNext();) {
					destCollection.add(performCast(it.next(), feature));
				}
			} else {
				destCollection.add(value);
			}
			return;
		}
		target.eSet(feature, value);
	}

	/**
	 * @generated
	 */
	protected Object performCast(Object value, ETypedElement targetType) {
		if (targetType.getEType() == null
				|| targetType.getEType().getInstanceClass() == null) {
			return value;
		}
		Class targetClass = targetType.getEType().getInstanceClass();
		if (value != null && value instanceof Number) {
			Number num = (Number) value;
			Class valClass = value.getClass();
			Class targetWrapperClass = targetClass;
			if (targetClass.isPrimitive()) {
				targetWrapperClass = EcoreUtil.wrapperClassFor(targetClass);
			}
			if (valClass.equals(targetWrapperClass)) {
				return value;
			}
			if (Number.class.isAssignableFrom(targetWrapperClass)) {
				if (targetWrapperClass.equals(Byte.class))
					return new Byte(num.byteValue());
				if (targetWrapperClass.equals(Integer.class))
					return new Integer(num.intValue());
				if (targetWrapperClass.equals(Short.class))
					return new Short(num.shortValue());
				if (targetWrapperClass.equals(Long.class))
					return new Long(num.longValue());
				if (targetWrapperClass.equals(BigInteger.class))
					return BigInteger.valueOf(num.longValue());
				if (targetWrapperClass.equals(Float.class))
					return new Float(num.floatValue());
				if (targetWrapperClass.equals(Double.class))
					return new Double(num.doubleValue());
				if (targetWrapperClass.equals(BigDecimal.class))
					return new BigDecimal(num.doubleValue());
			}
		}
		return value;
	}

	/**
	 * @generated
	 */
	public static final PtnetLoLAAbstractExpression createNullExpression(
			EClassifier context) {
		return new PtnetLoLAAbstractExpression(context) {

			protected Object doEvaluate(Object context, Map env) {
				// TODO - log entry about not provider available for this expression
				return null;
			}
		};
	}

	/**
	 * @generated
	 */
	public static class NoImplException extends RuntimeException {

		/**
		 * @generated
		 */
		public NoImplException(String message) {
			super(message);
		}
	}
}
