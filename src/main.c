#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include <uxhw.h>

#define MODEL_DRY_AIR_CONSTANT (287.05)
#define MODEL_MS_TO_KNOTS (1.94384)
#define MODEL_NEWTON_TO_KG (0.101972)

typedef struct {

	// Static parameters.
	double airfiol_base_area; // Meter^2
	double airfiol_geometry; // none, (C^2 - 1.0)

	// Sensors.
	double tolerance_air_temperature;
	double tolerance_pressure;

	// Dynamic parameters.
	double air_temperature; // Kelvin
	double pressure_absolute; // Pascal
	double pressure_pitot; // Pascal

	// To calculate.
	double airfoil_pressure_delta; // Pascal, Eq.1
	double lift_force; // Newton, Eq.2
	double air_density; // Pascal, Eq.3
	double air_speed; // Meter/Second, Eq.4

} Medel_s;

void model_init(
	Medel_s* ins,
	const double airfiol_base_area,
	const double airfiol_geometry,
	const double tolerance_air_temperature,
	const double tolerance_pressure
) {
	ins->airfiol_base_area = airfiol_base_area;
	ins->airfiol_geometry = (airfiol_geometry * airfiol_geometry - 1.0);
	ins->tolerance_air_temperature = tolerance_air_temperature;
	ins->tolerance_pressure = tolerance_pressure;
}

void model_update(
	Medel_s* ins,
	const double air_temperature_nominal,
	const double pressure_absolute_nominal,
	const double pressure_pitot_nominal
	) {

	ins->air_temperature = UxHwDoubleUniformDist(
		air_temperature_nominal * (1.0 - ins->tolerance_air_temperature),
		air_temperature_nominal * (1.0 + ins->tolerance_air_temperature)
	);

	ins->pressure_absolute = UxHwDoubleUniformDist(
		pressure_absolute_nominal * (1.0 - ins->tolerance_pressure),
		pressure_absolute_nominal * (1.0 + ins->tolerance_pressure)
	);

	ins->pressure_pitot = UxHwDoubleUniformDist(
		pressure_pitot_nominal * (1.0 - ins->tolerance_pressure),
		pressure_pitot_nominal * (1.0 + ins->tolerance_pressure)
	);

	ins->air_density = ins->pressure_absolute / (MODEL_DRY_AIR_CONSTANT * ins->air_temperature);
	ins->air_speed = sqrt((2.0 * (ins->pressure_pitot - ins->pressure_absolute)) / ins->air_density);
	ins->airfoil_pressure_delta = 0.5 * ins->air_density * (ins->air_speed * ins->air_speed) * ins->airfiol_geometry;
	ins->lift_force = ins->airfoil_pressure_delta * ins->airfiol_base_area;
}

void model_dump(const Medel_s* ins) {
	printf("air_density             : %lf(Kg/Meter^3)\n", ins->air_density);
	printf("air_speed               : %lf(Meter/Second) %lf(Knots)\n", ins->air_speed, ins->air_speed * MODEL_MS_TO_KNOTS);
	printf("airfoil_pressure_delta  : %lf(Pascal) \n", ins->airfoil_pressure_delta);
	printf("lift_force              : %lf(Newton) %lf(Kg)\n", ins->lift_force, ins->lift_force * MODEL_NEWTON_TO_KG);
}

int main(int argc, char**  argv) {

	// Cesna 172
	// Wing Area : 174 sq ft (16.17 M^2)
	// Max Landing Weight: 2400 Lb (1088.622 Kg)
	// Maximum Speed at Sea Level 124 knots
	// Normal Cruise Speed 111 knots
	// Stall Speed, Flaps Up	48 knots
	// Stall Speed, Flaps Down  40 knots
	// Absolute ceiling : 13,000 feet (3962.4 meters)
	// Airfoil geometry constant was grossly calculated by measuring the wing profile pictures. ~ 1.17
	// Sensors :
	// RTD Temperature Sensor : 0.4% uncertainty.
	// Absolute pressure sensor CPT6010 : 0.02% FS uncertainty.

	Medel_s cesna_172;
	model_init(&cesna_172, 16.17, 1.17, 0.004, 0.0002);

	printf("Cesna 172, cruise speed, altitude is close the sea level. \n");
	model_update(&cesna_172, 300, 101000, 102900);
	model_dump(&cesna_172);
	printf("\n");

	printf("Cesna 172, cruise speed, altitude is close to the absolute ceiling (4000 meters.) \n");
	model_update(&cesna_172, 270, 62500, 63800);
	model_dump(&cesna_172);

	return EXIT_SUCCESS;
}

