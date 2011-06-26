package org.st.scenarios.test;


import junit.framework.Assert;
import junit.framework.TestCase;
import junit.framework.TestSuite;

import org.cpntools.accesscpn.model.PetriNet;
import org.cpntools.accesscpn.model.exporter.DOMGenerator;
import org.junit.Test;
import org.st.scenarios.clsc.Chart;
import org.st.scenarios.clsc.Event;
import org.st.scenarios.clsc.Scenario;
import org.st.scenarios.clsc.Specification;
import org.st.scenarios.clsc.export.ExportToCPN;

public class TranslationTest extends TestSuite {
	
	public static String defaultTestDir = "./tests/testfiles";

	private static Specification createSpecification() {
		Specification spec = new Specification();
		
		// ---- S1
		Chart ems1_pre = new Chart();
		Chart ems1_main = new Chart();
		Event e1 = ems1_pre.addEvent("medic!avail");
		
		Event e2 = ems1_main.addEvent("medic,check");
		Event e3 = ems1_main.addEvent("EMS?avail");
		Event e4 = ems1_main.addEvent("EMS!location");
		Event e5 = ems1_main.addEvent("medic?location");
		ems1_main.addDependency(e2, e5);
		ems1_main.addDependency(e3, e4);
		ems1_main.addDependency(e4, e5);
		

		Scenario s1 = new Scenario("EMS1", ems1_pre, ems1_main);
		spec.scenarios.add(s1);

		// ---- S2
		Chart ems2_main = new Chart();
		
		Event f2 = ems2_main.addEvent("medic,check");
		Event f3 = ems2_main.addEvent("EMS?avail");
		Event f4 = ems2_main.addEvent("EMS!none");
		Event f5 = ems2_main.addEvent("medic?none");
		Event f6 = ems2_main.addEvent("medic!avail");
		ems2_main.addDependency(f2, f5);
		ems2_main.addDependency(f3, f4);
		ems2_main.addDependency(f4, f5);
		ems2_main.addDependency(f5, f6);
		
		Scenario s2 = new Scenario("EMS2", ems1_pre, ems2_main);
		spec.scenarios.add(s2);
		
		// ---- S0
		Chart ems0_main = new Chart();
		ems0_main.addEvent("medic!avail");
		Scenario s0 = new Scenario("EMS0", null, ems0_main);
		spec.scenarios.add(s0);
		
		return spec;
	}
	
	@Test
	public void translate_EMS() throws Exception {
		Specification spec = createSpecification();
		ExportToCPN e = new ExportToCPN(spec);
		PetriNet net = e.translate(true);

		Assert.assertNotNull(net);
		
		String testFileRoot = System.getProperty("test.testFileRoot", TranslationTest.defaultTestDir);
		DOMGenerator.export(net, testFileRoot+"/test_ems.cpn");
	}

	//public static void main(String[] args) {
	//	junit.textui.TestRunner.run(TranslationTest.class);
	//}
}
