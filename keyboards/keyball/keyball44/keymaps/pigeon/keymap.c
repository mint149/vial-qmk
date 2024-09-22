/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "quantum.h"

// レイヤー定義
#define _WINDOWS 0
#define _MAC 1
#define _LOWER 2
#define _RAISE 3
#define _ADJUST 4
#define _MOUSE 5
#define _SCROLL 6

// 長いキーをマクロに
#define SCRLTRG LT(_SCROLL,KC_BTN3)
#define NOSPACE _______
#define DELETED _______
#define NEXTTAB LCTL(KC_TAB)
#define PREVTAB LCTL(LSFT(KC_TAB))
#define NEXTXLS LCTL(KC_PGDN)
#define PREVXLS LCTL(KC_PGUP)
#define MOADJ MO(_ADJUST)

#define ANTI_SLEEP_INTERVAL 10000

// キーコード定義
enum custom_keycodes {
	WINDOWS = KEYBALL_SAFE_RANGE,
	MAC,
	IMEON,
	IMEOFF,
	TGL_JIS,
	TGL_AS,
	TGL_MS,
	TGL_LOCK,
	TGL_SCRL,
	TGL_OLED,
	// オートマウスレイヤに入る閾値を+1
	AMT_P1,
	// オートマウスレイヤに入る閾値を-1
	AMT_M1,
};

bool isInit = true;

bool imeOffOnly = false;
bool imeOnOnly = false;
bool isJisMode = true;
bool isScrollInvert = false;

bool isRecording = false;
bool isAntiSleepOn = false;
bool isMouseOnly = false;
bool isKeyDisabled = false;
int antiSleepIntervalCount = 0;
int pairingId = -1;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[_WINDOWS] = LAYOUT_universal(
		KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   ,                   KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_BSPC,
		KC_LCTL, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   ,                   KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_ENT ,
		KC_LSFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   ,                   KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_QUOT,
										    KC_LGUI, MOADJ, KC_LALT, IMEOFF , KC_SPC , KC_RGUI, IMEON  , NOSPACE, NOSPACE, MOADJ
	),

	[_MAC] = LAYOUT_universal(
		KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   ,                   KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_BSPC,
		KC_LGUI, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   ,                   KC_H   , KC_J   , KC_K   , KC_L   , KC_SCLN, KC_ENT ,
		KC_LSFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   ,                   KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, KC_QUOT,
										    KC_LALT, MOADJ  , KC_LGUI, IMEOFF , KC_SPC , KC_LCTL, IMEON  , NOSPACE, NOSPACE, MOADJ
	),

	[_LOWER] = LAYOUT_universal(
		KC_ESC ,TGL_OLED, KC_F2  , KC_F3  , KC_F4  , KC_F5  ,                   KC_1   , KC_2   , KC_3   , KC_4   , KC_5   , KC_DEL , 
		_______, KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 ,                   KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , _______, 
		_______, KC_F11 , KC_F12 , _______, _______, _______,                   KC_MINS, KC_EQL , KC_LBRC, KC_RBRC, KC_BSLS, KC_GRV ,
											_______, _______, _______, _______, _______, _______, _______, _______, _______, _______
	),

	[_RAISE] = LAYOUT_universal(
		KC_ESC , AMT_M1, AMT_P1, _______, _______, _______,                   KC_HOME, PREVXLS, NEXTXLS,  KC_END,TGL_OLED, KC_DEL , 
		_______, _______, _______, _______, _______, _______,                   KC_LEFT, KC_DOWN, KC_UP  ,KC_RIGHT, _______, _______, 
		_______, _______, _______, _______, _______, _______,                   KC_MINS, KC_EQL , KC_LBRC, KC_RBRC, KC_BSLS, KC_GRV ,
											DM_REC1, _______, _______, _______, _______, _______, _______, _______, _______, _______
	),

	[_ADJUST] = LAYOUT_universal(
		_______, ADV_ID0, WINDOWS, _______, _______, _______,                   _______, SEL_USB, ADV_ID0, ADV_ID1, ADV_ID2, ADV_ID3, 
		_______, AD_WO_L,TGL_SCRL, _______, _______, _______,                   _______, TGL_JIS, ADV_ID4, ADV_ID5, ADV_ID6, ADV_ID7, 
		TGL_AS , _______, _______, _______, _______, SEL_BLE,                   SEL_BLE,     MAC, _______, _______, _______, _______, 
											_______, _______, _______, _______, _______, _______, _______, _______, _______, TGL_LOCK
	),

	[_MOUSE] = LAYOUT_universal(
		_______, _______, _______, _______, _______, _______,                   _______, PREVXLS, NEXTXLS, _______, _______, _______, 
		_______, _______, _______, _______, _______, _______,                   PREVTAB, KC_BTN1, KC_BTN2, NEXTTAB, _______, _______, 
		_______, _______, _______, _______, _______, _______,                   _______, KC_BTN3, SCRLTRG, _______, _______, _______, 
											_______, _______, _______, _______, _______, _______, _______, _______, _______, _______
	),

	[_SCROLL] = LAYOUT_universal(
		_______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, 
		_______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, 
		_______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______, 
											_______, _______, _______, _______, _______, _______, _______, _______, _______, _______
	),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
	// Auto enable scroll mode when the highest layer is 3

	// keyball_set_scroll_mode(get_highest_layer(state) == 3);

	switch(get_highest_layer(remove_auto_mouse_layer(state, true))) {
		case _SCROLL:
			// Auto enable scroll mode when the highest layer is 3
			state = remove_auto_mouse_layer(state, false);
			set_auto_mouse_enable(false);
			keyball_set_scroll_mode(true);
			break;
		default:
			if(isMouseOnly){
				keyball_set_scroll_mode(false);
				break;
			}
			set_auto_mouse_enable(true);
			keyball_set_scroll_mode(false);
			break;
	}

	return state;
}

#ifdef OLED_ENABLE

#include "lib/oledkit/oledkit.h"

static const char *format_4d(int8_t d) {
    static char buf[5] = {0}; // max width (4) + NUL (1)
    char        lead   = ' ';
    if (d < 0) {
        d    = -d;
        lead = '-';
    }
    buf[3] = (d % 10) + '0';
    d /= 10;
    if (d == 0) {
        buf[2] = lead;
        lead   = ' ';
    } else {
        buf[2] = (d % 10) + '0';
        d /= 10;
    }
    if (d == 0) {
        buf[1] = lead;
        lead   = ' ';
    } else {
        buf[1] = (d % 10) + '0';
        d /= 10;
    }
    buf[0] = lead;
    return buf;
}

// サブ側OLEDにもキー入力情報を送るためのメソッド
bool should_process_keypress(void) { return true; }

// サブ側OLEDの表示制御
void oledkit_render_logo_user(void){
	// 1行目 ----------------------------------------------------------
	switch(pairingId){
		case 0:
			oled_write_P(PSTR("BT0:Slave  "), false);
			break;

		case 1:
			oled_write_P(PSTR("BT1:Macbook"), false);
			break;

		case 2:
			oled_write_P(PSTR("BT2:iPad   "), false);
			break;

		case 3:
			oled_write_P(PSTR("BT3:iPhone "), false);
			break;

		case 4:
			oled_write_P(PSTR("BT4:Fold   "), false);
			break;

		case 5:
			oled_write_P(PSTR("BT5:Work   "), false);
			break;

		case 6:
			oled_write_P(PSTR("BT6:AVP    "), false);
			break;

		case 7:
			oled_write_P(PSTR("BT7:WinPC  "), false);
			break;

		default:
		    oled_write_P(PSTR("USB:       "), false);
			break;
	}

	// 2行目 ----------------------------------------------------------
	oled_write_P(PSTR("Layer:"), false);
	switch (get_highest_layer(layer_state | default_layer_state)) {
		case _MAC:
			oled_write_P(PSTR("Mac   "), false);
			break;
		case _WINDOWS:
			oled_write_P(PSTR("Win   "), false);
			break;
		case _LOWER:
			oled_write_P(PSTR("Lower "), false);
			break;
		case _RAISE:
			oled_write_P(PSTR("Raise "), false);
			break;
		case _ADJUST:
			oled_write_P(PSTR("Adjust"), false);
			break;
		case _MOUSE:
			oled_write_P(PSTR("Mouse "), false);
			break;
		case _SCROLL:
			oled_write_P(PSTR("Scroll"), false);
			break;
		default:
			oled_write_P(PSTR("Undef "), false);
			break;
	}

	// 3行目 ----------------------------------------------------------
	// 4行目 ----------------------------------------------------------
}

// マウスキー判定となるキーを追加するためのメソッド
bool is_mouse_record_user(uint16_t keycode, keyrecord_t* record) {
    switch(keycode) {
        case KC_TAB:
            return true;
        default:
            return false;
    }
	return false;
}

// メイン側OLEDの表示制御
void oledkit_render_info_user(void) {
	if(isInit){
		set_auto_mouse_enable(true);
		keyball_set_scroll_mode(false);
		isInit = false;
	}

	// 1行目 ----------------------------------------------------------
	if(isScrollInvert){
    	oled_write_P(PSTR("SCRL:Rev  "), false);
	}else{
    	oled_write_P(PSTR("SCRL:Nml "), false);
	}

	switch(pairingId){
		case 0:
			oled_write_P(PSTR("BT0:Slave  "), false);
			break;

		case 1:
			oled_write_P(PSTR("BT1:Macbook"), false);
			break;

		case 2:
			oled_write_P(PSTR("BT2:iPad   "), false);
			break;

		case 3:
			oled_write_P(PSTR("BT3:iPhone "), false);
			break;

		case 4:
			oled_write_P(PSTR("BT4:Fold   "), false);
			break;

		case 5:
			oled_write_P(PSTR("BT5:Work   "), false);
			break;

		case 6:
			oled_write_P(PSTR("BT6:AVP    "), false);
			break;

		case 7:
			oled_write_P(PSTR("BT7:WinPC  "), false);
			break;

		default:
		    oled_write_P(PSTR("USB:       "), false);
			break;
	}

	// 2行目 ----------------------------------------------------------
	oled_write_P(PSTR("Layer:"), false);
	switch (get_highest_layer(layer_state | default_layer_state)) {
		case _MAC:
			oled_write_P(PSTR("Mac   "), false);
			break;
		case _WINDOWS:
			oled_write_P(PSTR("Win   "), false);
			break;
		case _LOWER:
			oled_write_P(PSTR("Lower "), false);
			break;
		case _RAISE:
			oled_write_P(PSTR("Raise "), false);
			break;
		case _ADJUST:
			oled_write_P(PSTR("Adjust"), false);
			break;
		case _MOUSE:
			oled_write_P(PSTR("Mouse "), false);
			break;
		case _SCROLL:
			oled_write_P(PSTR("Scroll"), false);
			break;
		default:
			oled_write_P(PSTR("Undef "), false);
			break;
	}

	oled_write_P(PSTR(" :"), false);

	if(isJisMode){
		oled_write_P(PSTR("JIS"), false);
	}else{
		oled_write_P(PSTR(" US"), false);
	}

	if(isRecording){
		oled_write_P(PSTR(" REC"), false);
	}else{
		oled_write_P(PSTR("    "), false);
	}

	// 3行目 ----------------------------------------------------------
	// オートマウスレイヤの閾値確認用
	oled_write_P(PSTR("AMT:"), false);
	uint8_t amThreshold = keyball_get_auto_mouse_threshold();
    oled_write(format_4d(amThreshold), false);

	oled_write_P(PSTR(" TM:"), false);
	uint8_t totalMove = keyball_get_total_move();
    oled_write(format_4d(totalMove), false);

	oled_write_P(PSTR(" ["), false);
	if(isAntiSleepOn){
		oled_write_P(PSTR("AS"), false);
	}else{
		oled_write_P(PSTR("  "), false);
	}
	oled_write_P(PSTR("/"), false);
	if(isKeyDisabled){
		oled_write_P(PSTR("IL"), false);
	}else{
		oled_write_P(PSTR("  "), false);
	}
	oled_write_P(PSTR("] "), false);
	
	// 4行目 ----------------------------------------------------------
    oled_write_P(PSTR("CP:"), false);
    oled_write(format_4d(keyball_get_cpi()) + 1, false);
    oled_write_P(PSTR("00 SD:"), false);
    oled_write_char(keyball_get_scroll_div(), false);
}
#endif

// マクロの記録を開始する時に起動されます。
void dynamic_macro_record_start_user(int8_t direction){
	isRecording = true;
}

// マクロの記録を停止した時に起動されます。
void dynamic_macro_record_end_user(int8_t direction){
	isRecording = false;
}

/* Copyright 2018-2020 eswai <@eswai>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
	OSで日本語キーボード(logical bit pairing)と設定/認識されているキーボードで、
	USキーキャップの文字、記号(typewriter pairing)を正しく出力する。
	例: Shift + 2 で @ を入力する
	変換された文字はキーリピートが無効です。
*/

// #include QMK_KEYBOARD_H
#include "keymap_japanese.h"

const uint16_t us2jis[][2] = {
	{KC_LPRN, JP_LPRN},
	{KC_RPRN, JP_RPRN},
	{KC_AT,   JP_AT},
	{KC_LBRC, JP_LBRC},
	{KC_RBRC, JP_RBRC},
	{KC_LCBR, JP_LCBR},
	{KC_RCBR, JP_RCBR},
	{KC_MINS, JP_MINS},
	{KC_EQL,  JP_EQL},
	{KC_BSLS, JP_BSLS},
	{KC_SCLN, JP_SCLN},
	{KC_QUOT, JP_QUOT},
	{KC_GRV,  JP_GRV},
	{KC_PLUS, JP_PLUS},
	{KC_COLN, JP_COLN},
	{KC_UNDS, JP_UNDS},
	{KC_PIPE, JP_PIPE},
	{KC_DQT,  JP_DQUO},
	{KC_ASTR, JP_ASTR},
	{KC_TILD, JP_TILD},
	{KC_AMPR, JP_AMPR},
	{KC_CIRC, JP_CIRC},
};

bool twpair_on_jis(uint16_t keycode, keyrecord_t *record) {
	if (!record->event.pressed) return true;

	uint16_t skeycode; // シフトビットを反映したキーコード
	bool lshifted = keyboard_report->mods & MOD_BIT(KC_LSFT); // シフトキーの状態
	bool rshifted = keyboard_report->mods & MOD_BIT(KC_RSFT);
	bool shifted = lshifted | rshifted;

	if (shifted) {
		skeycode = QK_LSFT | keycode;
	} else {
		skeycode = keycode;
	}

	for (int i = 0; i < sizeof(us2jis) / sizeof(us2jis[0]); i++) {
		if (us2jis[i][0] == skeycode) {
			unregister_code(KC_LSFT);
			unregister_code(KC_RSFT);
			if ((us2jis[i][1] & QK_LSFT) == QK_LSFT || (us2jis[i][1] & QK_RSFT) == QK_RSFT) {
				register_code(KC_LSFT);
				tap_code(us2jis[i][1]);
				unregister_code(KC_LSFT);
			} else {
				tap_code(us2jis[i][1]);
			}
			if (lshifted) register_code(KC_LSFT);
			if (rshifted) register_code(KC_RSFT);
			return false;
		}
	}

	return true;
}

void pointing_device_init_user(void) {
    set_auto_mouse_enable(true);
}

// トラックボールのセンサ値取得時に呼ばれるイベント
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
	if(!isScrollInvert){
	    mouse_report.v = mouse_report.v * -1;
	}

    return mouse_report;
}

void matrix_scan_user(void) {
	if (isAntiSleepOn) {
		if (antiSleepIntervalCount >= ANTI_SLEEP_INTERVAL) {
			send_string_with_delay("ASE\b\b\b", 200);
			antiSleepIntervalCount = 0;
		}
		antiSleepIntervalCount++;
	}
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
	// キー入力無効化機能
	if(isKeyDisabled){
		switch (keycode) {
			case IMEOFF:
				// Adjustレイヤーにロックキーを入れているため、キーロック中もレイヤ変更機能を動作させる必要がある
				// ひとまず通常時のレイヤ移動キーからIME制御機能を抜いてそのまま持ってきている
				// TODO:全く同じ処理なので関数化したほうがいい
				if (record->event.pressed) {
					layer_on(_LOWER);
					auto_mouse_layer_off();
					update_tri_layer(_LOWER, _RAISE, _ADJUST);
				} else {
					layer_off(_LOWER);
					auto_mouse_layer_off();
					update_tri_layer(_LOWER, _RAISE, _ADJUST);
				}
				return false;

			case IMEON:
				if (record->event.pressed) {
					layer_on(_RAISE);
					auto_mouse_layer_off();
					update_tri_layer(_LOWER, _RAISE, _ADJUST);
				} else {
					layer_off(_RAISE);
					auto_mouse_layer_off();
					update_tri_layer(_LOWER, _RAISE, _ADJUST);
				}
				return false;

			case TGL_LOCK:
				if (record->event.pressed) {
					isKeyDisabled = false;
				}
				return false;

			default:
				// ロックキー以外のあらゆるキーコードの入力をキャンセルする
				return false;
		}
	}

	// IMEキーの単押し判定用Switch
	switch (keycode) {
		case IMEOFF:
			imeOnOnly = false;
			break;
		case IMEON:
			imeOffOnly = false;
			break;
		default:
			imeOffOnly = false;
			imeOnOnly = false;
			break;
	}

	switch (keycode) {
		case MAC:
			if (record->event.pressed) {
				default_layer_set(1UL<<_MAC);
				isScrollInvert = true;
			}
			return false;

		case WINDOWS:
			if (record->event.pressed) {
				default_layer_set(1UL<<_WINDOWS);
				isScrollInvert = false;
			}
			return false;

		case IMEOFF:
			if (record->event.pressed) {
				imeOffOnly = true;
				layer_on(_LOWER);
				auto_mouse_layer_off();
				update_tri_layer(_LOWER, _RAISE, _ADJUST);
			} else {
				layer_off(_LOWER);
				auto_mouse_layer_off();
				update_tri_layer(_LOWER, _RAISE, _ADJUST);
		
				if (imeOffOnly) {
					switch (get_highest_layer(default_layer_state)) {
						case _WINDOWS:
							tap_code(KC_INT5);
							break;
						case _MAC: 
							tap_code(KC_LNG2);
							break;
						default:
							break;
					}
				}
				imeOffOnly = false;
			}
			return false;

		case IMEON:
			if (record->event.pressed) {
				imeOnOnly = true;
				layer_on(_RAISE);
				auto_mouse_layer_off();
				update_tri_layer(_LOWER, _RAISE, _ADJUST);
			} else {
				layer_off(_RAISE);
				auto_mouse_layer_off();
				update_tri_layer(_LOWER, _RAISE, _ADJUST);
		
				if (imeOnOnly) {
					switch (get_highest_layer(default_layer_state)) {
						case _WINDOWS:
							tap_code(KC_INT4);
							break;
						case _MAC: 
							tap_code(KC_LNG1);
							break;
						default:
							break;
					}
				}
				imeOnOnly = false;
			}
			return false;

		case TGL_JIS:
			if (record->event.pressed) {
				isJisMode = !isJisMode;
			}

			return false;

		case TGL_AS:
			if (record->event.pressed) {
				isAntiSleepOn = !isAntiSleepOn;
				antiSleepIntervalCount = 0;
			}

			return false;
		
		case TGL_MS:
			if (record->event.pressed) {
				if(isMouseOnly){
					isMouseOnly = false;
					set_auto_mouse_enable(true);
				}else{
					isMouseOnly = true;
					set_auto_mouse_enable(false);
					layer_on(_MOUSE);
				}
			}
			return false;

		case TGL_LOCK:
			if (record->event.pressed) {
				isKeyDisabled = true;
				layer_off(_LOWER);
				layer_off(_RAISE);
				layer_off(_ADJUST);
				auto_mouse_layer_off();
				update_tri_layer(_LOWER, _RAISE, _ADJUST);
			}
			return false;

		case TGL_SCRL:
			if (record->event.pressed) {
				isScrollInvert = !isScrollInvert;
			}
			return false;

		case TGL_OLED:
			if (record->event.pressed) {
				if(getIsOledEnable()){
					oled_off();
					setIsOledEnable(false);
				}else{
					setIsOledEnable(true);
					oled_on();
				}
			}
			return false;

		case AMT_M1:
			if (record->event.pressed) {
				keyball_set_auto_mouse_threshold(keyball_get_auto_mouse_threshold() - 1);
			}
			return true;

		case AMT_P1:
			if (record->event.pressed) {
				keyball_set_auto_mouse_threshold(keyball_get_auto_mouse_threshold() + 1);
			}
			return true;

		case AD_WO_L:
			return true;

		case ADV_ID0:
			pairingId = 0;
			return true;

		case ADV_ID1:
			pairingId = 1;
			return true;

		case ADV_ID2:
			pairingId = 2;
			return true;

		case ADV_ID3:
			pairingId = 3;
			return true;

		case ADV_ID4:
			pairingId = 4;
			return true;

		case ADV_ID5:
			pairingId = 5;
			return true;

		case ADV_ID6:
			pairingId = 6;
			return true;

		case ADV_ID7:
			pairingId = 7;
			return true;

		case SEL_BLE:
			return true;

		case SEL_USB:
			pairingId = -1;
			return true;

		default:
			if(isJisMode){
				return twpair_on_jis(keycode, record);
			}else{
				return true;
			}
	}

	return true;
}
