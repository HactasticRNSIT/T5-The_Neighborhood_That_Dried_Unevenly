#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_ZONES 100

typedef struct {
    int zone_id;
    double latitude;
    double longitude;

    double rainfall_mm;
    double humidity;
    double temperature;
    double wind_speed;
    double solar_radiation;
    double cloud_cover;

    double drain_density;
    double drain_capacity;
    int clogged_drain_reports;
    double drainage_efficiency;

    double impervious_surface_percent;

    char soil_type[20];
    double soil_absorption_score;

    double ndvi;
    double slope;
    double elevation;
    double groundwater_depth;

    int active_construction_sites;
    int excavation_count;
    int debris_complaints;
    int construction_disturbance;

    double previous_rainfall_24h;
    double previous_rainfall_7d;
    double previous_rain_saturation;

    double evaporation_potential;

    double drying_time_hr;
    double moisture_risk_score;
    char risk_level[20];

} ZoneData;

double random_double(double min, double max) {
    return min + ((double)rand() / RAND_MAX) * (max - min);
}

int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

double clamp(double value, double min, double max) {
    if (value < min) {
        return min;
    }

    if (value > max) {
        return max;
    }

    return value;
}

void assign_soil_type(ZoneData *zone) {
    int choice = random_int(1, 4);

    if (choice == 1) {
        strcpy(zone->soil_type, "sandy");
        zone->soil_absorption_score = 0.9;
    } else if (choice == 2) {
        strcpy(zone->soil_type, "loamy");
        zone->soil_absorption_score = 0.7;
    } else if (choice == 3) {
        strcpy(zone->soil_type, "mixed");
        zone->soil_absorption_score = 0.5;
    } else {
        strcpy(zone->soil_type, "clay");
        zone->soil_absorption_score = 0.2;
    }
}

void calculate_engineered_features(ZoneData *zone) {
    zone->drainage_efficiency =
        (zone->drain_density * zone->drain_capacity)
        - (0.15 * zone->clogged_drain_reports);

    zone->construction_disturbance =
        zone->active_construction_sites
        + zone->excavation_count
        + zone->debris_complaints;

    zone->previous_rain_saturation =
        zone->previous_rainfall_24h
        + (0.4 * zone->previous_rainfall_7d);

    zone->evaporation_potential =
        (0.03 * zone->temperature)
        + (0.04 * zone->wind_speed)
        + (0.002 * zone->solar_radiation)
        - (0.03 * zone->humidity)
        - (0.01 * zone->cloud_cover);
}

void calculate_drying_time(ZoneData *zone) {
    zone->drying_time_hr =
        2
        + (0.08 * zone->rainfall_mm)
        + (0.05 * zone->humidity)
        + (0.06 * zone->impervious_surface_percent)
        - (1.4 * zone->drainage_efficiency)
        - (3.0 * zone->soil_absorption_score)
        - (0.5 * zone->slope)
        - (0.18 * zone->groundwater_depth)
        + (0.18 * zone->construction_disturbance)
        + (0.025 * zone->previous_rain_saturation)
        - (1.2 * zone->evaporation_potential);

    if (zone->drying_time_hr < 1) {
        zone->drying_time_hr = 1;
    }
}

void calculate_moisture_risk_score(ZoneData *zone) {
    double drainage_risk = clamp((3.0 - zone->drainage_efficiency) * 25.0, 0, 100);
    double impervious_risk = clamp(zone->impervious_surface_percent, 0, 100);
    double soil_risk = clamp((1.0 - zone->soil_absorption_score) * 100.0, 0, 100);
    double slope_risk = clamp((5.0 - zone->slope) * 20.0, 0, 100);
    double groundwater_risk = clamp((5.0 - zone->groundwater_depth) * 20.0, 0, 100);
    double construction_risk = clamp(zone->construction_disturbance * 5.0, 0, 100);
    double humidity_risk = clamp((zone->humidity - 50.0) * 2.0, 0, 100);
    double previous_rain_risk = clamp(zone->previous_rain_saturation * 0.7, 0, 100);

    zone->moisture_risk_score =
        (0.25 * drainage_risk)
        + (0.20 * impervious_risk)
        + (0.15 * soil_risk)
        + (0.15 * slope_risk)
        + (0.10 * groundwater_risk)
        + (0.07 * construction_risk)
        + (0.04 * humidity_risk)
        + (0.04 * previous_rain_risk);

    zone->moisture_risk_score = clamp(zone->moisture_risk_score, 0, 100);
}

void classify_risk(ZoneData *zone) {
    if (zone->moisture_risk_score < 35) {
        strcpy(zone->risk_level, "Low");
    } else if (zone->moisture_risk_score < 65) {
        strcpy(zone->risk_level, "Medium");
    } else {
        strcpy(zone->risk_level, "High");
    }
}

void process_zone(ZoneData *zone) {
    calculate_engineered_features(zone);
    calculate_drying_time(zone);
    calculate_moisture_risk_score(zone);
    classify_risk(zone);
}

void generate_zone_data(ZoneData zones[]) {
    double base_lat = 12.9716;
    double base_lon = 77.5946;

    for (int i = 0; i < NUM_ZONES; i++) {
        zones[i].zone_id = i + 1;

        zones[i].latitude = base_lat + random_double(-0.08, 0.08);
        zones[i].longitude = base_lon + random_double(-0.08, 0.08);

        zones[i].rainfall_mm = random_double(10, 90);
        zones[i].humidity = random_double(55, 98);
        zones[i].temperature = random_double(20, 34);
        zones[i].wind_speed = random_double(1, 20);
        zones[i].solar_radiation = random_double(100, 900);
        zones[i].cloud_cover = random_double(10, 95);

        zones[i].drain_density = random_double(0.5, 5.0);
        zones[i].drain_capacity = random_double(0.4, 1.0);
        zones[i].clogged_drain_reports = random_int(0, 15);

        zones[i].impervious_surface_percent = random_double(25, 95);

        assign_soil_type(&zones[i]);

        zones[i].ndvi = random_double(0.1, 0.75);
        zones[i].slope = random_double(0.2, 8.0);
        zones[i].elevation = random_double(800, 950);
        zones[i].groundwater_depth = random_double(0.5, 10.0);

        zones[i].active_construction_sites = random_int(0, 8);
        zones[i].excavation_count = random_int(0, 6);
        zones[i].debris_complaints = random_int(0, 10);

        zones[i].previous_rainfall_24h = random_double(0, 50);
        zones[i].previous_rainfall_7d = random_double(10, 250);

        process_zone(&zones[i]);
    }
}

void save_to_csv(ZoneData zones[]) {
    FILE *file = fopen("moisture_data.csv", "w");

    if (file == NULL) {
        printf("Error: Could not create CSV file.\n");
        return;
    }

    fprintf(file,
        "zone_id,latitude,longitude,rainfall_mm,humidity,temperature,wind_speed,"
        "solar_radiation,cloud_cover,drain_density,drain_capacity,"
        "clogged_drain_reports,drainage_efficiency,impervious_surface_percent,"
        "soil_type,soil_absorption_score,ndvi,slope,elevation,groundwater_depth,"
        "active_construction_sites,excavation_count,debris_complaints,"
        "construction_disturbance,previous_rainfall_24h,previous_rainfall_7d,"
        "previous_rain_saturation,evaporation_potential,drying_time_hr,"
        "moisture_risk_score,risk_level\n"
    );

    for (int i = 0; i < NUM_ZONES; i++) {
        fprintf(file,
            "%d,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,"
            "%.2f,%.2f,%d,%.2f,%.2f,%s,%.2f,%.2f,%.2f,%.2f,%.2f,"
            "%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%s\n",

            zones[i].zone_id,
            zones[i].latitude,
            zones[i].longitude,
            zones[i].rainfall_mm,
            zones[i].humidity,
            zones[i].temperature,
            zones[i].wind_speed,
            zones[i].solar_radiation,
            zones[i].cloud_cover,
            zones[i].drain_density,
            zones[i].drain_capacity,
            zones[i].clogged_drain_reports,
            zones[i].drainage_efficiency,
            zones[i].impervious_surface_percent,
            zones[i].soil_type,
            zones[i].soil_absorption_score,
            zones[i].ndvi,
            zones[i].slope,
            zones[i].elevation,
            zones[i].groundwater_depth,
            zones[i].active_construction_sites,
            zones[i].excavation_count,
            zones[i].debris_complaints,
            zones[i].construction_disturbance,
            zones[i].previous_rainfall_24h,
            zones[i].previous_rainfall_7d,
            zones[i].previous_rain_saturation,
            zones[i].evaporation_potential,
            zones[i].drying_time_hr,
            zones[i].moisture_risk_score,
            zones[i].risk_level
        );
    }

    fclose(file);
    printf("CSV file 'moisture_data.csv' created successfully.\n");
}

void save_high_risk_report(ZoneData zones[]) {
    FILE *file = fopen("high_risk_report.csv", "w");

    if (file == NULL) {
        printf("Error: Could not create high risk report.\n");
        return;
    }

    fprintf(file,
        "zone_id,moisture_risk_score,drying_time_hr,rainfall_mm,humidity,"
        "drainage_efficiency,clogged_drain_reports,impervious_surface_percent,"
        "soil_type,soil_absorption_score,slope,groundwater_depth,"
        "construction_disturbance,previous_rain_saturation,risk_level\n"
    );

    for (int i = 0; i < NUM_ZONES; i++) {
        if (strcmp(zones[i].risk_level, "High") == 0) {
            fprintf(file,
                "Z%03d,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%s,%.2f,%.2f,%.2f,%d,%.2f,%s\n",
                zones[i].zone_id,
                zones[i].moisture_risk_score,
                zones[i].drying_time_hr,
                zones[i].rainfall_mm,
                zones[i].humidity,
                zones[i].drainage_efficiency,
                zones[i].clogged_drain_reports,
                zones[i].impervious_surface_percent,
                zones[i].soil_type,
                zones[i].soil_absorption_score,
                zones[i].slope,
                zones[i].groundwater_depth,
                zones[i].construction_disturbance,
                zones[i].previous_rain_saturation,
                zones[i].risk_level
            );
        }
    }

    fclose(file);
    printf("High risk report saved as 'high_risk_report.csv'.\n");
}

void display_zone_summary(ZoneData zone) {
    printf("\n=================================================\n");
    printf("ZONE Z%03d MOISTURE REPORT\n", zone.zone_id);
    printf("=================================================\n");

    printf("Location: %.6f, %.6f\n", zone.latitude, zone.longitude);
    printf("Rainfall: %.2f mm\n", zone.rainfall_mm);
    printf("Humidity: %.2f %%\n", zone.humidity);
    printf("Temperature: %.2f C\n", zone.temperature);
    printf("Drying Time: %.2f hours\n", zone.drying_time_hr);
    printf("Moisture Risk Score: %.2f / 100\n", zone.moisture_risk_score);
    printf("Risk Level: %s\n", zone.risk_level);

    printf("\nInfrastructure and Environment:\n");
    printf("Drainage Efficiency: %.2f\n", zone.drainage_efficiency);
    printf("Clogged Drain Reports: %d\n", zone.clogged_drain_reports);
    printf("Impervious Surface: %.2f %%\n", zone.impervious_surface_percent);
    printf("Soil Type: %s\n", zone.soil_type);
    printf("Soil Absorption Score: %.2f\n", zone.soil_absorption_score);
    printf("Vegetation Index NDVI: %.2f\n", zone.ndvi);
    printf("Slope: %.2f\n", zone.slope);
    printf("Groundwater Depth: %.2f m\n", zone.groundwater_depth);
    printf("Construction Disturbance Score: %d\n", zone.construction_disturbance);
    printf("Previous Rain Saturation: %.2f\n", zone.previous_rain_saturation);
    printf("Evaporation Potential: %.2f\n", zone.evaporation_potential);
}

void explain_causes_and_recommendations(ZoneData zone) {
    printf("\nLikely Causes:\n");

    int cause_found = 0;

    if (zone.drainage_efficiency < 1.0) {
        printf("- Low drainage efficiency may be slowing water removal.\n");
        cause_found = 1;
    }

    if (zone.clogged_drain_reports > 7) {
        printf("- High clogged drain reports indicate possible blocked drainage.\n");
        cause_found = 1;
    }

    if (zone.impervious_surface_percent > 70) {
        printf("- High concrete/asphalt coverage prevents water absorption.\n");
        cause_found = 1;
    }

    if (zone.soil_absorption_score < 0.4) {
        printf("- Low soil absorption, likely due to clay-heavy soil.\n");
        cause_found = 1;
    }

    if (zone.slope < 2.0) {
        printf("- Low slope makes natural water flow difficult.\n");
        cause_found = 1;
    }

    if (zone.groundwater_depth < 2.0) {
        printf("- Shallow groundwater may keep the surface damp for longer.\n");
        cause_found = 1;
    }

    if (zone.construction_disturbance > 10) {
        printf("- Construction activity may be blocking or altering water flow.\n");
        cause_found = 1;
    }

    if (zone.previous_rain_saturation > 100) {
        printf("- Previous rainfall saturation means the area was already wet.\n");
        cause_found = 1;
    }

    if (zone.humidity > 85) {
        printf("- High humidity slows evaporation.\n");
        cause_found = 1;
    }

    if (!cause_found) {
        printf("- No major visible cause detected. Continue monitoring.\n");
    }

    printf("\nRecommended Actions:\n");

    if (zone.drainage_efficiency < 1.0 || zone.clogged_drain_reports > 7) {
        printf("- Clean and inspect local drainage channels.\n");
    }

    if (zone.impervious_surface_percent > 70) {
        printf("- Add permeable pavements, recharge pits, or rain gardens.\n");
    }

    if (zone.soil_absorption_score < 0.4) {
        printf("- Improve soil infiltration using recharge wells or soil treatment.\n");
    }

    if (zone.slope < 2.0) {
        printf("- Improve stormwater diversion in low-slope areas.\n");
    }

    if (zone.groundwater_depth < 2.0) {
        printf("- Monitor groundwater and consider subsurface drainage.\n");
    }

    if (zone.construction_disturbance > 10) {
        printf("- Remove construction debris and create temporary runoff paths.\n");
    }

    if (zone.previous_rain_saturation > 100) {
        printf("- Prepare temporary pumping after repeated rainfall.\n");
    }

    if (zone.humidity > 85) {
        printf("- Increase monitoring during humid months.\n");
    }
}

void show_high_risk_zones(ZoneData zones[]) {
    printf("\n=================================================\n");
    printf("HIGH RISK ZONES\n");
    printf("=================================================\n");

    int count = 0;

    for (int i = 0; i < NUM_ZONES; i++) {
        if (strcmp(zones[i].risk_level, "High") == 0) {
            printf(
                "Z%03d | Risk Score: %.2f | Drying Time: %.2f hr | Rainfall: %.2f mm | Drainage: %.2f | Impervious: %.2f%%\n",
                zones[i].zone_id,
                zones[i].moisture_risk_score,
                zones[i].drying_time_hr,
                zones[i].rainfall_mm,
                zones[i].drainage_efficiency,
                zones[i].impervious_surface_percent
            );
            count++;
        }
    }

    if (count == 0) {
        printf("No high-risk zones found.\n");
    } else {
        printf("\nTotal high-risk zones: %d\n", count);
    }
}

void show_city_summary(ZoneData zones[]) {
    int low_count = 0;
    int medium_count = 0;
    int high_count = 0;

    double total_drying_time = 0;
    double total_risk_score = 0;

    double max_risk = -1;
    int max_risk_zone = -1;

    double max_drying_time = -1;
    int max_drying_zone = -1;

    for (int i = 0; i < NUM_ZONES; i++) {
        total_drying_time += zones[i].drying_time_hr;
        total_risk_score += zones[i].moisture_risk_score;

        if (strcmp(zones[i].risk_level, "Low") == 0) {
            low_count++;
        } else if (strcmp(zones[i].risk_level, "Medium") == 0) {
            medium_count++;
        } else if (strcmp(zones[i].risk_level, "High") == 0) {
            high_count++;
        }

        if (zones[i].moisture_risk_score > max_risk) {
            max_risk = zones[i].moisture_risk_score;
            max_risk_zone = zones[i].zone_id;
        }

        if (zones[i].drying_time_hr > max_drying_time) {
            max_drying_time = zones[i].drying_time_hr;
            max_drying_zone = zones[i].zone_id;
        }
    }

    printf("\n=================================================\n");
    printf("CITY MOISTURE SUMMARY\n");
    printf("=================================================\n");

    printf("Total Zones: %d\n", NUM_ZONES);
    printf("Low Risk Zones: %d\n", low_count);
    printf("Medium Risk Zones: %d\n", medium_count);
    printf("High Risk Zones: %d\n", high_count);

    printf("Average Drying Time: %.2f hours\n", total_drying_time / NUM_ZONES);
    printf("Average Moisture Risk Score: %.2f / 100\n", total_risk_score / NUM_ZONES);

    printf("Highest Risk Zone: Z%03d with score %.2f\n", max_risk_zone, max_risk);
    printf("Slowest Drying Zone: Z%03d with %.2f hours\n", max_drying_zone, max_drying_time);
}

void show_priority_zones(ZoneData zones[]) {
    ZoneData sorted[NUM_ZONES];

    for (int i = 0; i < NUM_ZONES; i++) {
        sorted[i] = zones[i];
    }

    for (int i = 0; i < NUM_ZONES - 1; i++) {
        for (int j = 0; j < NUM_ZONES - i - 1; j++) {
            if (sorted[j].moisture_risk_score < sorted[j + 1].moisture_risk_score) {
                ZoneData temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    printf("\n=================================================\n");
    printf("TOP 10 PRIORITY ZONES FOR MUNICIPAL ACTION\n");
    printf("=================================================\n");

    printf("%-10s %-15s %-15s %-15s %-15s\n",
        "Zone",
        "Risk Score",
        "Drying Time",
        "Drainage",
        "Impervious"
    );

    for (int i = 0; i < 10 && i < NUM_ZONES; i++) {
        printf("Z%03d      %-15.2f %-15.2f %-15.2f %-15.2f\n",
            sorted[i].zone_id,
            sorted[i].moisture_risk_score,
            sorted[i].drying_time_hr,
            sorted[i].drainage_efficiency,
            sorted[i].impervious_surface_percent
        );
    }
}

void compare_two_zones(ZoneData zones[]) {
    int id1, id2;

    printf("\nEnter first zone number: ");
    scanf("%d", &id1);

    printf("Enter second zone number: ");
    scanf("%d", &id2);

    if (id1 < 1 || id1 > NUM_ZONES || id2 < 1 || id2 > NUM_ZONES) {
        printf("Invalid zone number.\n");
        return;
    }

    ZoneData a = zones[id1 - 1];
    ZoneData b = zones[id2 - 1];

    printf("\n=================================================\n");
    printf("ZONE COMPARISON: Z%03d vs Z%03d\n", id1, id2);
    printf("=================================================\n");

    printf("%-30s %-15s %-15s\n", "Factor", "Zone A", "Zone B");
    printf("%-30s %-15.2f %-15.2f\n", "Rainfall mm", a.rainfall_mm, b.rainfall_mm);
    printf("%-30s %-15.2f %-15.2f\n", "Drying Time hr", a.drying_time_hr, b.drying_time_hr);
    printf("%-30s %-15.2f %-15.2f\n", "Risk Score", a.moisture_risk_score, b.moisture_risk_score);
    printf("%-30s %-15s %-15s\n", "Risk Level", a.risk_level, b.risk_level);
    printf("%-30s %-15.2f %-15.2f\n", "Drainage Efficiency", a.drainage_efficiency, b.drainage_efficiency);
    printf("%-30s %-15.2f %-15.2f\n", "Impervious Surface %", a.impervious_surface_percent, b.impervious_surface_percent);
    printf("%-30s %-15s %-15s\n", "Soil Type", a.soil_type, b.soil_type);
    printf("%-30s %-15.2f %-15.2f\n", "Soil Absorption", a.soil_absorption_score, b.soil_absorption_score);
    printf("%-30s %-15.2f %-15.2f\n", "Slope", a.slope, b.slope);
    printf("%-30s %-15.2f %-15.2f\n", "Groundwater Depth", a.groundwater_depth, b.groundwater_depth);
    printf("%-30s %-15d %-15d\n", "Construction Score", a.construction_disturbance, b.construction_disturbance);

    printf("\nQuick Insight:\n");

    if (a.moisture_risk_score > b.moisture_risk_score) {
        printf("Z%03d has higher moisture risk than Z%03d.\n", a.zone_id, b.zone_id);
    } else if (b.moisture_risk_score > a.moisture_risk_score) {
        printf("Z%03d has higher moisture risk than Z%03d.\n", b.zone_id, a.zone_id);
    } else {
        printf("Both zones have almost equal moisture risk.\n");
    }
}

void generate_html_dashboard(ZoneData zones[]) {
    FILE *file = fopen("dashboard.html", "w");

    if (file == NULL) {
        printf("Error: Could not create dashboard.html.\n");
        return;
    }

    int low_count = 0;
    int medium_count = 0;
    int high_count = 0;

    double total_drying_time = 0;
    double total_risk_score = 0;

    for (int i = 0; i < NUM_ZONES; i++) {
        total_drying_time += zones[i].drying_time_hr;
        total_risk_score += zones[i].moisture_risk_score;

        if (strcmp(zones[i].risk_level, "Low") == 0) {
            low_count++;
        } else if (strcmp(zones[i].risk_level, "Medium") == 0) {
            medium_count++;
        } else {
            high_count++;
        }
    }

    ZoneData sorted[NUM_ZONES];

    for (int i = 0; i < NUM_ZONES; i++) {
        sorted[i] = zones[i];
    }

    for (int i = 0; i < NUM_ZONES - 1; i++) {
        for (int j = 0; j < NUM_ZONES - i - 1; j++) {
            if (sorted[j].moisture_risk_score < sorted[j + 1].moisture_risk_score) {
                ZoneData temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    fprintf(file, "<!DOCTYPE html>\n");
    fprintf(file, "<html>\n");
    fprintf(file, "<head>\n");
    fprintf(file, "<title>Urban Moisture Intelligence System</title>\n");

    fprintf(file, "<style>\n");
    fprintf(file, "body { font-family: Arial, sans-serif; margin: 30px; background: #f4f8fb; }\n");
    fprintf(file, "h1 { color: #0b3d5c; }\n");
    fprintf(file, ".cards { display: flex; gap: 20px; margin-bottom: 30px; flex-wrap: wrap; }\n");
    fprintf(file, ".card { background: white; padding: 20px; border-radius: 12px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); width: 220px; }\n");
    fprintf(file, ".value { font-size: 28px; font-weight: bold; color: #0b3d5c; }\n");
    fprintf(file, "table { width: 100%%; border-collapse: collapse; background: white; }\n");
    fprintf(file, "th, td { padding: 10px; border-bottom: 1px solid #ddd; text-align: left; }\n");
    fprintf(file, "th { background: #0b3d5c; color: white; }\n");
    fprintf(file, ".Low { color: green; font-weight: bold; }\n");
    fprintf(file, ".Medium { color: orange; font-weight: bold; }\n");
    fprintf(file, ".High { color: red; font-weight: bold; }\n");
    fprintf(file, "</style>\n");

    fprintf(file, "</head>\n");
    fprintf(file, "<body>\n");

    fprintf(file, "<h1>Urban Moisture Intelligence System</h1>\n");
    fprintf(file, "<p>C-powered prototype for identifying slow-drying neighborhoods after rainfall.</p>\n");

    fprintf(file, "<div class='cards'>\n");
    fprintf(file, "<div class='card'><p>Total Zones</p><div class='value'>%d</div></div>\n", NUM_ZONES);
    fprintf(file, "<div class='card'><p>Low Risk Zones</p><div class='value'>%d</div></div>\n", low_count);
    fprintf(file, "<div class='card'><p>Medium Risk Zones</p><div class='value'>%d</div></div>\n", medium_count);
    fprintf(file, "<div class='card'><p>High Risk Zones</p><div class='value'>%d</div></div>\n", high_count);
    fprintf(file, "<div class='card'><p>Avg Drying Time</p><div class='value'>%.2f hr</div></div>\n", total_drying_time / NUM_ZONES);
    fprintf(file, "<div class='card'><p>Avg Risk Score</p><div class='value'>%.2f</div></div>\n", total_risk_score / NUM_ZONES);
    fprintf(file, "</div>\n");

    fprintf(file, "<h2>Top Priority Zones</h2>\n");

    fprintf(file, "<table>\n");
    fprintf(file, "<tr><th>Zone</th><th>Risk Score</th><th>Drying Time</th><th>Risk Level</th><th>Main Concern</th></tr>\n");

    for (int i = 0; i < 15 && i < NUM_ZONES; i++) {
        char main_concern[100];

        if (sorted[i].drainage_efficiency < 1.0) {
            strcpy(main_concern, "Poor drainage");
        } else if (sorted[i].impervious_surface_percent > 70) {
            strcpy(main_concern, "High concrete coverage");
        } else if (sorted[i].soil_absorption_score < 0.4) {
            strcpy(main_concern, "Low soil absorption");
        } else if (sorted[i].slope < 2.0) {
            strcpy(main_concern, "Low slope");
        } else if (sorted[i].construction_disturbance > 10) {
            strcpy(main_concern, "Construction disturbance");
        } else {
            strcpy(main_concern, "Mixed factors");
        }

        fprintf(file,
            "<tr><td>Z%03d</td><td>%.2f</td><td>%.2f hr</td><td class='%s'>%s</td><td>%s</td></tr>\n",
            sorted[i].zone_id,
            sorted[i].moisture_risk_score,
            sorted[i].drying_time_hr,
            sorted[i].risk_level,
            sorted[i].risk_level,
            main_concern
        );
    }

    fprintf(file, "</table>\n");

    fprintf(file, "<h2>What This Prototype Does</h2>\n");
    fprintf(file, "<p>This system uses rainfall, humidity, drainage, land surface, soil, slope, groundwater, construction, and previous rainfall saturation data to estimate drying behavior after rainfall.</p>\n");

    fprintf(file, "<h2>Recommended Municipal Use</h2>\n");
    fprintf(file, "<p>Authorities can use the priority zones to schedule drain cleaning, improve water diversion, monitor low-lying areas, and plan permeable infrastructure.</p>\n");

    fprintf(file, "</body>\n");
    fprintf(file, "</html>\n");

    fclose(file);

    printf("HTML dashboard generated as 'dashboard.html'.\n");
}

void manual_prediction() {
    ZoneData zone;

    zone.zone_id = 999;
    zone.latitude = 0;
    zone.longitude = 0;

    printf("\n=================================================\n");
    printf("MANUAL MOISTURE RISK PREDICTION\n");
    printf("=================================================\n");

    printf("Enter rainfall in mm: ");
    scanf("%lf", &zone.rainfall_mm);

    printf("Enter humidity percentage: ");
    scanf("%lf", &zone.humidity);

    printf("Enter temperature in Celsius: ");
    scanf("%lf", &zone.temperature);

    printf("Enter wind speed: ");
    scanf("%lf", &zone.wind_speed);

    printf("Enter solar radiation: ");
    scanf("%lf", &zone.solar_radiation);

    printf("Enter cloud cover percentage: ");
    scanf("%lf", &zone.cloud_cover);

    printf("Enter drain density: ");
    scanf("%lf", &zone.drain_density);

    printf("Enter drain capacity between 0 and 1: ");
    scanf("%lf", &zone.drain_capacity);

    printf("Enter clogged drain reports: ");
    scanf("%d", &zone.clogged_drain_reports);

    printf("Enter impervious surface percentage: ");
    scanf("%lf", &zone.impervious_surface_percent);

    printf("Choose soil type:\n");
    printf("1. Sandy\n");
    printf("2. Loamy\n");
    printf("3. Mixed\n");
    printf("4. Clay\n");
    printf("Enter choice: ");

    int soil_choice;
    scanf("%d", &soil_choice);

    if (soil_choice == 1) {
        strcpy(zone.soil_type, "sandy");
        zone.soil_absorption_score = 0.9;
    } else if (soil_choice == 2) {
        strcpy(zone.soil_type, "loamy");
        zone.soil_absorption_score = 0.7;
    } else if (soil_choice == 3) {
        strcpy(zone.soil_type, "mixed");
        zone.soil_absorption_score = 0.5;
    } else {
        strcpy(zone.soil_type, "clay");
        zone.soil_absorption_score = 0.2;
    }

    printf("Enter NDVI vegetation index between 0 and 1: ");
    scanf("%lf", &zone.ndvi);

    printf("Enter slope value: ");
    scanf("%lf", &zone.slope);

    printf("Enter elevation: ");
    scanf("%lf", &zone.elevation);

    printf("Enter groundwater depth in meters: ");
    scanf("%lf", &zone.groundwater_depth);

    printf("Enter active construction sites: ");
    scanf("%d", &zone.active_construction_sites);

    printf("Enter excavation count: ");
    scanf("%d", &zone.excavation_count);

    printf("Enter debris complaints: ");
    scanf("%d", &zone.debris_complaints);

    printf("Enter previous rainfall in last 24 hours: ");
    scanf("%lf", &zone.previous_rainfall_24h);

    printf("Enter previous rainfall in last 7 days: ");
    scanf("%lf", &zone.previous_rainfall_7d);

    process_zone(&zone);

    display_zone_summary(zone);
    explain_causes_and_recommendations(zone);
}

void menu(ZoneData zones[]) {
    int choice;
    int zone_number;

    while (1) {
        printf("\n\n==============================\n");
        printf("URBAN MOISTURE INTELLIGENCE SYSTEM\n");
        printf("==============================\n");
        printf("1. Show report for a zone\n");
        printf("2. Show all high-risk zones\n");
        printf("3. Compare two zones\n");
        printf("4. Show city summary\n");
        printf("5. Show top priority zones\n");
        printf("6. Save data to CSV\n");
        printf("7. Save high-risk report CSV\n");
        printf("8. Generate HTML dashboard\n");
        printf("9. Manual moisture risk prediction\n");
        printf("10. Exit\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter zone number between 1 and %d: ", NUM_ZONES);
                scanf("%d", &zone_number);

                if (zone_number < 1 || zone_number > NUM_ZONES) {
                    printf("Invalid zone number.\n");
                } else {
                    display_zone_summary(zones[zone_number - 1]);
                    explain_causes_and_recommendations(zones[zone_number - 1]);
                }
                break;

            case 2:
                show_high_risk_zones(zones);
                break;

            case 3:
                compare_two_zones(zones);
                break;

            case 4:
                show_city_summary(zones);
                break;

            case 5:
                show_priority_zones(zones);
                break;

            case 6:
                save_to_csv(zones);
                break;

            case 7:
                save_high_risk_report(zones);
                break;

            case 8:
                generate_html_dashboard(zones);
                break;

            case 9:
                manual_prediction();
                break;

            case 10:
                printf("Exiting program.\n");
                return;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

int main() {
    srand(time(NULL));

    ZoneData zones[NUM_ZONES];

    generate_zone_data(zones);

    printf("Synthetic neighborhood moisture data generated successfully.\n");

    save_to_csv(zones);
    save_high_risk_report(zones);
    generate_html_dashboard(zones);

    menu(zones);

    return 0;
}