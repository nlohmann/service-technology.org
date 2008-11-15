package hub.top.adaptiveSystem.diagram.providers;

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
public class AdaptiveSystemParserProvider extends AbstractProvider implements
		IParserProvider {

	/**
	 * @generated
	 */
	private IParser ocletName_4016Parser;

	/**
	 * @generated
	 */
	private IParser getOcletName_4016Parser() {
		if (ocletName_4016Parser == null) {
			ocletName_4016Parser = createOcletName_4016Parser();
		}
		return ocletName_4016Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createOcletName_4016Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getOclet_Name(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser ocletQuantorOrientation_4017Parser;

	/**
	 * @generated
	 */
	private IParser getOcletQuantorOrientation_4017Parser() {
		if (ocletQuantorOrientation_4017Parser == null) {
			ocletQuantorOrientation_4017Parser = createOcletQuantorOrientation_4017Parser();
		}
		return ocletQuantorOrientation_4017Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createOcletQuantorOrientation_4017Parser() {
		EAttribute[] features = new EAttribute[] {
				hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
						.getOclet_Quantor(),
				hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
						.getOclet_Orientation(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		parser.setViewPattern("[{0} - {1}]");
		parser.setEditorPattern("[{0} - {1}]");
		parser.setEditPattern("[{0} - {1}]");
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionName_4001Parser;

	/**
	 * @generated
	 */
	private IParser getConditionName_4001Parser() {
		if (conditionName_4001Parser == null) {
			conditionName_4001Parser = createConditionName_4001Parser();
		}
		return conditionName_4001Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionName_4001Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionTemp_4002Parser;

	/**
	 * @generated
	 */
	private IParser getConditionTemp_4002Parser() {
		if (conditionTemp_4002Parser == null) {
			conditionTemp_4002Parser = createConditionTemp_4002Parser();
		}
		return conditionTemp_4002Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionTemp_4002Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Temp(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionToken_4003Parser;

	/**
	 * @generated
	 */
	private IParser getConditionToken_4003Parser() {
		if (conditionToken_4003Parser == null) {
			conditionToken_4003Parser = createConditionToken_4003Parser();
		}
		return conditionToken_4003Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionToken_4003Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getCondition_Token(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser eventName_4004Parser;

	/**
	 * @generated
	 */
	private IParser getEventName_4004Parser() {
		if (eventName_4004Parser == null) {
			eventName_4004Parser = createEventName_4004Parser();
		}
		return eventName_4004Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createEventName_4004Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser eventTemp_4005Parser;

	/**
	 * @generated
	 */
	private IParser getEventTemp_4005Parser() {
		if (eventTemp_4005Parser == null) {
			eventTemp_4005Parser = createEventTemp_4005Parser();
		}
		return eventTemp_4005Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createEventTemp_4005Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Temp(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionName_4006Parser;

	/**
	 * @generated
	 */
	private IParser getConditionName_4006Parser() {
		if (conditionName_4006Parser == null) {
			conditionName_4006Parser = createConditionName_4006Parser();
		}
		return conditionName_4006Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionName_4006Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionTemp_4007Parser;

	/**
	 * @generated
	 */
	private IParser getConditionTemp_4007Parser() {
		if (conditionTemp_4007Parser == null) {
			conditionTemp_4007Parser = createConditionTemp_4007Parser();
		}
		return conditionTemp_4007Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionTemp_4007Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Temp(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionToken_4008Parser;

	/**
	 * @generated
	 */
	private IParser getConditionToken_4008Parser() {
		if (conditionToken_4008Parser == null) {
			conditionToken_4008Parser = createConditionToken_4008Parser();
		}
		return conditionToken_4008Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionToken_4008Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getCondition_Token(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser eventName_4009Parser;

	/**
	 * @generated
	 */
	private IParser getEventName_4009Parser() {
		if (eventName_4009Parser == null) {
			eventName_4009Parser = createEventName_4009Parser();
		}
		return eventName_4009Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createEventName_4009Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser eventTemp_4010Parser;

	/**
	 * @generated
	 */
	private IParser getEventTemp_4010Parser() {
		if (eventTemp_4010Parser == null) {
			eventTemp_4010Parser = createEventTemp_4010Parser();
		}
		return eventTemp_4010Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createEventTemp_4010Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Temp(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionName_4011Parser;

	/**
	 * @generated
	 */
	private IParser getConditionName_4011Parser() {
		if (conditionName_4011Parser == null) {
			conditionName_4011Parser = createConditionName_4011Parser();
		}
		return conditionName_4011Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionName_4011Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionTemp_4012Parser;

	/**
	 * @generated
	 */
	private IParser getConditionTemp_4012Parser() {
		if (conditionTemp_4012Parser == null) {
			conditionTemp_4012Parser = createConditionTemp_4012Parser();
		}
		return conditionTemp_4012Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionTemp_4012Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Temp(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser conditionToken_4013Parser;

	/**
	 * @generated
	 */
	private IParser getConditionToken_4013Parser() {
		if (conditionToken_4013Parser == null) {
			conditionToken_4013Parser = createConditionToken_4013Parser();
		}
		return conditionToken_4013Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createConditionToken_4013Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getCondition_Token(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser eventName_4014Parser;

	/**
	 * @generated
	 */
	private IParser getEventName_4014Parser() {
		if (eventName_4014Parser == null) {
			eventName_4014Parser = createEventName_4014Parser();
		}
		return eventName_4014Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createEventName_4014Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Name(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser eventTemp_4015Parser;

	/**
	 * @generated
	 */
	private IParser getEventTemp_4015Parser() {
		if (eventTemp_4015Parser == null) {
			eventTemp_4015Parser = createEventTemp_4015Parser();
		}
		return eventTemp_4015Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createEventTemp_4015Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getNode_Temp(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser arcToConditionWeight_6001Parser;

	/**
	 * @generated
	 */
	private IParser getArcToConditionWeight_6001Parser() {
		if (arcToConditionWeight_6001Parser == null) {
			arcToConditionWeight_6001Parser = createArcToConditionWeight_6001Parser();
		}
		return arcToConditionWeight_6001Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createArcToConditionWeight_6001Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getArc_Weight(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	private IParser arcToEventWeight_6002Parser;

	/**
	 * @generated
	 */
	private IParser getArcToEventWeight_6002Parser() {
		if (arcToEventWeight_6002Parser == null) {
			arcToEventWeight_6002Parser = createArcToEventWeight_6002Parser();
		}
		return arcToEventWeight_6002Parser;
	}

	/**
	 * @generated
	 */
	protected IParser createArcToEventWeight_6002Parser() {
		EAttribute[] features = new EAttribute[] { hub.top.adaptiveSystem.AdaptiveSystemPackage.eINSTANCE
				.getArc_Weight(), };
		hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser parser = new hub.top.adaptiveSystem.diagram.parsers.MessageFormatParser(
				features);
		return parser;
	}

	/**
	 * @generated
	 */
	protected IParser getParser(int visualID) {
		switch (visualID) {
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart.VISUAL_ID:
			return getOcletName_4016Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletQuantorOrientationEditPart.VISUAL_ID:
			return getOcletQuantorOrientation_4017Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPNameEditPart.VISUAL_ID:
			return getConditionName_4001Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTempEditPart.VISUAL_ID:
			return getConditionTemp_4002Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPTokenEditPart.VISUAL_ID:
			return getConditionToken_4003Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPNameEditPart.VISUAL_ID:
			return getEventName_4004Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPTempEditPart.VISUAL_ID:
			return getEventTemp_4005Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetNameEditPart.VISUAL_ID:
			return getConditionName_4006Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTempEditPart.VISUAL_ID:
			return getConditionTemp_4007Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetTokenEditPart.VISUAL_ID:
			return getConditionToken_4008Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetNameEditPart.VISUAL_ID:
			return getEventName_4009Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetTempEditPart.VISUAL_ID:
			return getEventTemp_4010Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetNameEditPart.VISUAL_ID:
			return getConditionName_4011Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTempEditPart.VISUAL_ID:
			return getConditionTemp_4012Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetTokenEditPart.VISUAL_ID:
			return getConditionToken_4013Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart.VISUAL_ID:
			return getEventName_4014Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetTempEditPart.VISUAL_ID:
			return getEventTemp_4015Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionWeightEditPart.VISUAL_ID:
			return getArcToConditionWeight_6001Parser();
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventWeightEditPart.VISUAL_ID:
			return getArcToEventWeight_6002Parser();
		}
		return null;
	}

	/**
	 * @generated
	 */
	public IParser getParser(IAdaptable hint) {
		String vid = (String) hint.getAdapter(String.class);
		if (vid != null) {
			return getParser(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getVisualID(vid));
		}
		View view = (View) hint.getAdapter(View.class);
		if (view != null) {
			return getParser(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
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
			if (hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes
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
