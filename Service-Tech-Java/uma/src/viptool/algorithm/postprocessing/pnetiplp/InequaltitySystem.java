package viptool.algorithm.postprocessing.pnetiplp;

import java.util.Vector;

import lpsolve.LpSolve;
import lpsolve.LpSolveException;

//TODO: kommentieren und überarbeiten

/**
 * This class is a supporting class for solving the inequality system to
 * identify implicit given places.
 */
public class InequaltitySystem {

	// Der Vorbereichs-Vector
	private Vector			pk;

	// Der Nachbereichs-Vector
	private Vector			pg;

	private PlaceDataFrame	place_;

	public InequaltitySystem(Vector plkl, Vector plgr, PlaceDataFrame place) {
		pk = plkl;
		pg = plgr;
		place_ = place;
	}

	/**
	 * This method solves the inequality system by using a variant of the
	 * simplex algorithm.
	 */
	public int execute() throws LpSolveException {
		
		LpSolve lp;
		
		int Ncol = 0;
		int j = 0;
		int ret = 0;
		
		// Vector All fügt die Vor-und Nachbereichsvektoren zusammen
		Vector All = new Vector();

		// Ncol ist die Spaltenzahl des Systems
		Ncol = pk.size() + pg.size();

		// baue das System zeilenweise

		// colno: Nr. der Spalte
		int[] colno = new int[Ncol];
		// row: jeweils eine Zeile
		double[] row = new double[Ncol];

		// Baut All zusammen
		for (int a = 0; a < pk.size(); a++) {
			All.add((PlaceDataFrame) pk.get(a)); /* first column */
		}

		for (int a = pk.size(); a < Ncol; a++) {
			All.add((PlaceDataFrame) pg.get(a - pk.size())); /* first column */
		}

		// Ugl-System wird gebaut:
		lp = LpSolve.makeLp(0, Ncol);
		if (lp.getLp() == 0)
			ret = 1; /* kein neues System möglich (Fehler) */

		if (ret == 0) {
			/* Variablenbenennung für evtl. Debugging */
			for (int a = 1; a < Ncol + 1; a++) {
				String s = "x" + "" + a;
				lp.setColName(a, s);

			}

			lp.setAddRowmode(true); /*
									 * makes building the model faster if it is
									 * done rows by row
									 */

			/* Konstruiere Zeile 1 */

			for (int a = 0; a < All.size(); a++) {
				colno[a] = a + 1; /* first column */
				row[a] = ((PlaceDataFrame) All.get(a)).getInitialMarking();
			}

			j = Ncol;

			/* Zeile ins System adden */

			lp
					.addConstraintex(j, row, colno, LpSolve.LE, place_
							.getInitialMarking());
		}

		for (int k = 0; k < place_.getTransitionCount(); k++) {
			if (ret == 0) {
				/* Konstruire Zeile 2 */

				for (int a = 0; a < All.size(); a++) {
					colno[a] = a + 1;
					row[a] = ((PlaceDataFrame) All.get(a))
							.getIncomingArcWeight(k);
				}

				j = Ncol;

				/* Zeile ins System adden */
				lp.addConstraintex(j, row, colno, LpSolve.LE, place_
						.getIncomingArcWeight(k));
			}
		}

		for (int k = 0; k < place_.getTransitionCount(); k++) {
			if (ret == 0) {
				/* Konstruire Zeile 3 */

				for (int a = 0; a < All.size(); a++) {
					colno[a] = a + 1; /* first column */
					row[a] = ((PlaceDataFrame) All.get(a))
							.getOutgoingArcWeight(k);
				}

				j = Ncol;

				/* Zeile ins System adden */
				lp.addConstraintex(j, row, colno, LpSolve.GE, place_
						.getOutgoingArcWeight(k));
			}
		}

		for (int c = 0; c < Ncol; c++) {
			if (ret == 0) {
				/* Zusatzbedingung: Koeffizienten grgl. Null */
				j = Ncol;

				for (int a = 0; a < All.size(); a++) {
					if (c != a) {
						colno[a] = a + 1; /* first column */
						row[a] = 0;
					}
					if (c == a) {
						colno[a] = a + 1; /* first column */
						row[a] = 1;
					}
				}

				/* Zeile ins System adden */
				lp.addConstraintex(j, row, colno, LpSolve.GE, 0);
			}
		}

		if (ret == 0) {
			lp.setAddRowmode(false);

			/*
			 * Allg. Funktionsbedingung (0 x + 0 y + ... ) (der Algorithmus
			 * braucht sie)
			 */
			j = Ncol;

			for (int a = 0; a < All.size(); a++) {
				colno[a] = a + 1; /* first column */
				row[a] = 0;
			}

			lp.setObjFnex(j, row, colno);
		}

		if (ret == 0) {
			/* set the object direction to maximize */
			lp.setMaxim();

			/*
			 * just out of curioucity, now generate the model in lp format in
			 * file model.lp
			 */
			lp.writeLp("model.lp");

			/* I only want to see important messages on screen while solving */
			lp.setVerbose(LpSolve.IMPORTANT);

			/* Now let lpsolve calculate a solution */
			ret = lp.solve();

			if (ret == LpSolve.OPTIMAL)
				ret = 0;
			else
				ret = 5;
		}

		lp.getVariables(row);

		/* Freimachen des Lösungssystems: */
		if (lp.getLp() != 0)
			lp.deleteLp();

		return (ret);
	}

}
