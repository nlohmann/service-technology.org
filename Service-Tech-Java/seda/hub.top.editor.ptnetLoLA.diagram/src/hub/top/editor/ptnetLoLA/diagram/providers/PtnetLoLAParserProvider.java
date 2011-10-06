package hub.top.editor.ptnetLoLA.diagram.providers;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.common.core.service.AbstractProvider;
import org.eclipse.gmf.runtime.common.core.service.IOperation;
import org.eclipse.gmf.runtime.common.ui.services.parser.GetParserOperation;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParser;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParserProvider;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.ui.services.parser.ParserHintAdapter;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class PtnetLoLAParserProvider extends AbstractProvider implements
		IParserProvider {

	/**
	 * @generated
	 */
	private IParser transitionName_5001Parser;

	/**
	 * @generated
	 */
	private IParser getTransitionName_5001Parser() {
		if (transitionName_5001Parser == null) {
			transitionName_5001Parser = createTransitionName_5001Parser();
		}
		return transitionName_5001Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createTransitionName_5001Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser placeName_5002Parser;

	/**
	 * @generated
	 */
	private IParser getPlaceName_5002Parser() {
		if (placeName_5002Parser == null) {
			placeName_5002Parser = createPlaceName_5002Parser();
		}
		return placeName_5002Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createPlaceName_5002Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser placeToken_5003Parser;

	/**
	 * @generated
	 */
	private IParser getPlaceToken_5003Parser() {
		if (placeToken_5003Parser == null) {
			placeToken_5003Parser = createPlaceToken_5003Parser();
		}
		return placeToken_5003Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createPlaceToken_5003Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getPlace_Token(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser arcToPlaceWeight_6001Parser;

	/**
	 * @generated
	 */
	private IParser getArcToPlaceWeight_6001Parser() {
		if (arcToPlaceWeight_6001Parser == null) {
			arcToPlaceWeight_6001Parser = createArcToPlaceWeight_6001Parser();
		}
		return arcToPlaceWeight_6001Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createArcToPlaceWeight_6001Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getArc_Weight(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser arcToTransitionWeight_6002Parser;

	/**
	 * @generated
	 */
	private IParser getArcToTransitionWeight_6002Parser() {
		if (arcToTransitionWeight_6002Parser == null) {
			arcToTransitionWeight_6002Parser = createArcToTransitionWeight_6002Parser();
		}
		return arcToTransitionWeight_6002Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createArcToTransitionWeight_6002Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.editor.ptnetLoLA.PtnetLoLAPackage.eINSTANCE
				.getArc_Weight(), };
		hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser parser = new hub.top.editor.ptnetLoLA.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	protected IParser getParser(int visualID) {
		switch (visualID) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart.VISUAL_ID:
			return getTransitionName_5001Parser();
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceNameEditPart.VISUAL_ID:
			return getPlaceName_5002Parser();
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceTokenEditPart.VISUAL_ID:
			return getPlaceToken_5003Parser();
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceWeightEditPart.VISUAL_ID:
			return getArcToPlaceWeight_6001Parser();
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionWeightEditPart.VISUAL_ID:
			return getArcToTransitionWeight_6002Parser();
		}
		return null;
	}

	/**
	 * @generated
	 */
	public IParser getParser(IAdaptable hint) {
		String vid = (String) hint.getAdapter(String.class);
		if (vid != null) {
			return getParser(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getVisualID(vid));
		}
		View view = (View) hint.getAdapter(View.class);
		if (view != null) {
			return getParser(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getVisualID(view));
		}
		return null;
	}

	/**
	 * @generated
	 */
	public boolean provides(IOperation operation) {
		if (operation instanceof GetParserOperation) {
			IAdaptable hint = ((GetParserOperation) operation).getHint();
			if (hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes
					.getElement(hint) == null) {
				return false;
			}
			return getParser(hint) != null;
		}
		return false;
	}

	/**
	 * @generated
	 */
	public static class HintAdapter extends ParserHintAdapter {

		/**
		 * @generated
		 */
		private final IElementType elementType;

		/**
		 * @generated
		 */
		public HintAdapter(IElementType type, EObject object, String parserHint) {
			super(object, parserHint);
			assert type != null;
			elementType = type;
		}

		/**
		 * @generated
		 */
		public Object getAdapter(Class adapter) {
			if (IElementType.class.equals(adapter)) {
				return elementType;
			}
			return super.getAdapter(adapter);
		}
	}

}
