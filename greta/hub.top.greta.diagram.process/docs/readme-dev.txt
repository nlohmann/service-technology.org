This file provides developer information for hub.top.GRETA.diagram.process.

This package is generated from  hub.top.GRETA/model/adaptiveProcess.gmfgen.
This GMF generator model is a restricted model of
hub.top.GRETA/model/adaptiveSystem.gmfgen. In essence, identifiers, IDs and
Java class names are kept compatible to allow sharing of the same Java code
base between hub.top.GRETA.diagram and hub.top.GRETA.diagram.process.

Though, some essential changes had to be made and the following dependencies
exist.

The packages
- hub.top.adaptiveProcess.diagram.navigator
- hub.top.adaptiveProcess.diagram.parser
- hub.top.adaptiveProcess.diagram.part
- hub.top.adaptiveProcess.diagram.preferences
- hub.top.adaptiveProcess.diagram.providers
- hub.top.adaptiveProcess.diagram.sheet
- hub.top.adaptiveProcess.diagram.view.factories

and the classes
- hub.top.adaptiveProcess.diagram.edit.parts.AP_AdaptiveSystemProcessEditPartFactory
- hub.top.adaptiveProcess.diagram.edit.parts.AP_ProcessEditPart

received a separate naming to ensure that hub.top.GRETA.diagram.process
runs safely in its own domain. All remaining classes
(in hub.top.adaptiveSystem.diagram.edit.*) got exactly the same names as
in hub.top.GRETA.diagram. The GMF gen model ensures that identifiers, IDs,
and Java class names match. 

By importing the dependency hub.top.GRETA.diagram into
hub.top.GRETA.diagram.process, the classes of hub.top.GRETA.diagram are
made available to hub.top.GRETA.diagram for re-use. For instance, the
EditParts in hub.top.adaptiveProcess.diagram.edit.parts have been removed
in this distributed, so the original Java classes with all non-generated
modifications are used in the process viewer.

You may re-generate these Java files and remove the hub.top.GRETA.diagram
dependency to implement your own visualization. The other remaining Java
files in hub.top.adaptiveSystem.diagram.edit.* can be removed (so the
hub.top.GRETA.diagram-files are used) or modified.

NOTE: As long as the hub.top.GRETA.diagram dependency is imported, Java
classes with same package and name from hub.top.GRETA.diagram will be
preferred over hub.top.GRETA.diagram.process.