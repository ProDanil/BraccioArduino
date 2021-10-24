#ifndef CONTROLLERX1_HPP
#define CONTROLLERX1_HPP

struct ControllerX1 {
    enum class ValBase {
        V0 = 0,
        V90 = 90,
        V120 = 120
    };
    enum class ValShoulder {
        V65 = 65,
        V80 = 80,
        V100 = 100,
        V115 = 115,
        V140 = 140
    };
    enum class ValElbow {
        V0 = 0,
        V180 = 180
    };
    enum class ValWristVer {
        V10 = 10,
        V15 = 15,
        V165 = 165,
        V170 = 170
    };
    enum class ValWristRot {
        V0 = 0,
        V90 = 90
    };
    enum class ValGripper {
        V15 = 15,
        V73 = 73
    };

    enum State {
        BEGIN,
        S1,
        S2,
        S3,
        S4,
        S5,
        S6,
        S7,
        S8,
        S9,
        S10,
        S11,
        S12,
        S13,
        __SAME__
    } state = BEGIN;

    struct Input {
        bool want_cargo_on_out;
        bool is_acquired;
        bool is_done_all;

        bool operator==(const Input& other) const {
            return want_cargo_on_out == other.want_cargo_on_out &&
                   is_acquired == other.is_acquired &&
                   is_done_all == other.is_done_all;
        }
    };

    struct Out {
        bool want_to_release;
        bool want_to_acquire;
        ValBase base;
        ValShoulder shoulder;
        ValElbow elbow;
        ValWristVer wrist_ver;
        ValWristRot wrist_rot;
        ValGripper gripper;
    } out;

    String state2string(State s) {
        switch (s) {
            case BEGIN:
                return "BEGIN";
            case S1:
                return "S1";
            case S2:
                return "S2";
            case S3:
                return "S3";
            case S4:
                return "S4";
            case S5:
                return "S5";
            case S6:
                return "S6";
            case S7:
                return "S7";
            case S8:
                return "S8";
            case S9:
                return "S9";
            case S10:
                return "S10";
            case S11:
                return "S11";
            case S12:
                return "S12";
            case S13:
                return "S13";
            case __SAME__:
                return "__SAME__";
            default:
                return "";
        }
    }

    bool go_step(Input input) {
        bool want_cargo_on_out = input.want_cargo_on_out;
        bool is_acquired = input.is_acquired;
        bool is_done_all = input.is_done_all;

        State next_state = __SAME__;

        // Compute the next state
        if (0) {
        } else if (state == BEGIN) {
            next_state = S1;
        } else if (state == S1 && (!is_acquired)) {
            next_state = S2;
        } else if (state == S2 && (want_cargo_on_out)) {
            next_state = S3;
        } else if (state == S3 && (is_done_all)) {
            next_state = S4;
        } else if (state == S4 && (is_done_all)) {
            next_state = S5;
        } else if (state == S5 && (is_done_all)) {
            next_state = S6;
        } else if (state == S6 && (is_done_all)) {
            next_state = S7;
        } else if (state == S7 && (is_acquired)) {
            next_state = S8;
        } else if (state == S8 && (is_done_all)) {
            next_state = S9;
        } else if (state == S9 && (is_done_all)) {
            next_state = S10;
        } else if (state == S10 && (is_done_all)) {
            next_state = S11;
        } else if (state == S11 && (is_done_all)) {
            next_state = S12;
        } else if (state == S12 && (is_done_all)) {
            next_state = S13;
        } else if (state == S13 && (is_done_all)) {
            next_state = S2;
        }

        // Compute outputs
        if (0) {
        } else if (next_state == S1) {
            out.want_to_release = false;
            out.want_to_acquire = false;
        } else if (next_state == S2) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V90;
            out.shoulder = ValShoulder::V140;
            out.elbow = ValElbow::V0;
            out.wrist_ver = ValWristVer::V10;
            out.wrist_rot = ValWristRot::V0;
            out.gripper = ValGripper::V15;
        } else if (next_state == S3) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V90;
            out.shoulder = ValShoulder::V115;
            out.elbow = ValElbow::V0;
            out.wrist_ver = ValWristVer::V15;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V15;
        } else if (next_state == S4) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V90;
            out.shoulder = ValShoulder::V100;
            out.elbow = ValElbow::V0;
            out.wrist_ver = ValWristVer::V10;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V15;
        } else if (next_state == S5) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V90;
            out.shoulder = ValShoulder::V100;
            out.elbow = ValElbow::V0;
            out.wrist_ver = ValWristVer::V10;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V73;
        } else if (next_state == S6) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V90;
            out.shoulder = ValShoulder::V115;
            out.elbow = ValElbow::V0;
            out.wrist_ver = ValWristVer::V15;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V73;
        } else if (next_state == S7) {
            out.want_to_release = false;
            out.want_to_acquire = true;
            out.base = ValBase::V90;
            out.shoulder = ValShoulder::V115;
            out.elbow = ValElbow::V0;
            out.wrist_ver = ValWristVer::V15;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V73;
        } else if (next_state == S8) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V120;
            out.shoulder = ValShoulder::V65;
            out.elbow = ValElbow::V180;
            out.wrist_ver = ValWristVer::V165;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V73;
        } else if (next_state == S9) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V120;
            out.shoulder = ValShoulder::V80;
            out.elbow = ValElbow::V180;
            out.wrist_ver = ValWristVer::V170;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V73;
        } else if (next_state == S10) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V120;
            out.shoulder = ValShoulder::V80;
            out.elbow = ValElbow::V180;
            out.wrist_ver = ValWristVer::V170;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V15;
        } else if (next_state == S11) {
            out.want_to_release = false;
            out.want_to_acquire = false;
            out.base = ValBase::V120;
            out.shoulder = ValShoulder::V65;
            out.elbow = ValElbow::V180;
            out.wrist_ver = ValWristVer::V165;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V15;
        } else if (next_state == S12) {
            out.want_to_release = true;
            out.want_to_acquire = false;
            out.base = ValBase::V120;
            out.shoulder = ValShoulder::V65;
            out.elbow = ValElbow::V180;
            out.wrist_ver = ValWristVer::V165;
            out.wrist_rot = ValWristRot::V90;
            out.gripper = ValGripper::V15;
        } else if (next_state == S13) {
            out.want_to_release = true;
            out.want_to_acquire = false;
            out.base = ValBase::V90;
            out.shoulder = ValShoulder::V140;
            out.elbow = ValElbow::V0;
            out.wrist_ver = ValWristVer::V10;
            out.wrist_rot = ValWristRot::V0;
            out.gripper = ValGripper::V15;
        }

        // Update the state
        if (next_state != __SAME__) {
            state = next_state;
        }

        // return is_active
        return (next_state != __SAME__);
    }
};

#endif  // CONTROLLERX1_HPP
