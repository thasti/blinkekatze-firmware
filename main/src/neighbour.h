#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <esp_err.h>
#include <esp_now.h>

#include "list.h"
#include "wireless.h"

typedef struct neighbour_advertisement {
	uint8_t packet_type;
	int64_t uptime_us;
	int64_t global_clock_us;
} __attribute__((packed)) neighbour_advertisement_t;

typedef struct neighbour_status_packet {
	uint8_t packet_type;
	int8_t battery_soc_percent;
	int16_t battery_voltage_mv;
	int16_t battery_current_ma;
	int16_t battery_temperature_0_1k;
	int16_t battery_time_to_empty_min;
	int16_t battery_full_charge_capacity_mah;
	int8_t battery_soh_percent;
} __attribute__((packed)) neighbour_status_packet_t;

typedef struct neighbour_static_info_packet {
	uint8_t packet_type;
	char ap_password[WIRELESS_AP_PASSWORD_LENGTH];
	char firmware_version[32];
	uint8_t firmware_sha256_hash[32];
} __attribute__((packed)) neighbour_static_info_packet_t;

typedef struct neighbour {
	list_head_t list;
	uint8_t address[ESP_NOW_ETH_ALEN];
	int64_t last_local_adv_rx_timestamp_us;
	int64_t local_to_remote_time_offset;
	neighbour_advertisement_t last_advertisement;
	neighbour_status_packet_t last_status;
	neighbour_static_info_packet_t last_static_info;
	int rssi;
} neighbour_t;

void neighbour_init(void);
const neighbour_t *neighbour_find_by_address(const uint8_t *address);
esp_err_t neighbour_update(const uint8_t *address, int64_t timestamp_us, const neighbour_advertisement_t *adv);
esp_err_t neighbour_rx(const wireless_packet_t *packet);
void neighbour_housekeeping();
int64_t neighbour_get_global_clock();
int64_t neighbour_get_global_clock_and_source(neighbour_t **src);
esp_err_t neighbour_update_rssi(const uint8_t *address, int rssi);
int64_t neighbour_remote_to_local_time(const neighbour_t *neigh, int64_t remote_timestamp);
int64_t neighbour_get_uptime(const neighbour_t *neigh);
bool neighbour_has_neighbours(void);
void neighbour_print_list(void);
void neighbour_update_status(const neighbour_t *neigh, const neighbour_status_packet_t *status);
void neighbour_update_static_info(const neighbour_t *neigh, const neighbour_static_info_packet_t *static_info);
