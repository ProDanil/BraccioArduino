#ifndef Controller2_hpp
#define Controller2_hpp

struct ControllerX2 {
    enum struct AngleBase {
        A0 = 0,
        A60 = 60,
        A90 = 90,
        A120 = 120,
        A180 = 180
    };
    enum struct AngleShoulder {
        A65 = 65,
        A90 = 90,
        A100 = 100,
        A115 = 115,
        A140 = 140
    };
    enum struct AngleElbow {
        A0 = 0,
        A180 = 180
    };
    enum struct AngleWristVer {
        A10 = 10,
        A15 = 15,
        A165 = 165
    };
    enum struct AngleWristRot {
        A0 = 0,
        A90 = 90
    };
    enum struct AngleGripper {
        A15 = 15,
        A73 = 73
    };

    enum State {
        BEGIN,
        GO_WAIT,
        WAIT,
        GO_SAFE_Z1,
        GO_UP_Z1,
        GO_LOW_Z1,
        GO_PICKUP_Z1,
        GO_UP_CLENCHED_Z1,
        GO_SAFE_CLENCHED_Z1,
        GO_SAFE_Z2,
        GO_UP_Z2,
        GO_LOW_Z2,
        GO_PICKUP_Z2,
        GO_UP_CLENCHED_Z2,
        GO_SAFE_CLENCHED_Z2,
        GO_SAFE_Z13,
        GO_SAFE_Z23,
        GO_UP_CLENCHED_Z3,
        GO_LOW_CLENCHED_Z3,
        GO_DROP_Z3,
        GO_UP_Z3,
        __SAME__
    } state = BEGIN;

    struct Input {
        bool cargo_is_processed_Z1;
        bool cargo_is_processed_Z2;
        bool is_done_m1;
        bool is_done_m2;
        bool is_done_m3;
        bool is_done_m4;
        bool is_done_m5;
        bool is_done_m6;

        bool operator==(const Input& other) const {
            return cargo_is_processed_Z1 == other.cargo_is_processed_Z1 &&
                   cargo_is_processed_Z2 == other.cargo_is_processed_Z2 &&
                   is_done_m1 == other.is_done_m1 &&
                   is_done_m2 == other.is_done_m2 &&
                   is_done_m3 == other.is_done_m3 &&
                   is_done_m4 == other.is_done_m4 &&
                   is_done_m5 == other.is_done_m5 &&
                   is_done_m6 == other.is_done_m6;
        }
    };

    struct Out {
        /* TODO: initial values */
        AngleBase go_base;
        AngleShoulder go_shoulder;
        AngleElbow go_elbow;
        AngleWristVer go_wrist_ver;
        AngleWristRot go_wrist_rot;
        AngleGripper go_gripper;
    } out;

    String state2string(State s) {
        switch (s) {
            case BEGIN:
                return "BEGIN";
            case GO_WAIT:
                return "GO_WAIT";
            case WAIT:
                return "WAIT";
            case GO_SAFE_Z1:
                return "GO_SAFE_Z1";
            case GO_UP_Z1:
                return "GO_UP_Z1";
            case GO_LOW_Z1:
                return "GO_LOW_Z1";
            case GO_PICKUP_Z1:
                return "GO_PICKUP_Z1";
            case GO_UP_CLENCHED_Z1:
                return "GO_UP_CLENCHED_Z1";
            case GO_SAFE_CLENCHED_Z1:
                return "GO_SAFE_CLENCHED_Z1";
            case GO_SAFE_Z2:
                return "GO_SAFE_Z2";
            case GO_UP_Z2:
                return "GO_UP_Z2";
            case GO_LOW_Z2:
                return "GO_LOW_Z2";
            case GO_PICKUP_Z2:
                return "GO_PICKUP_Z2";
            case GO_UP_CLENCHED_Z2:
                return "GO_UP_CLENCHED_Z2";
            case GO_SAFE_CLENCHED_Z2:
                return "GO_SAFE_CLENCHED_Z2";
            case GO_SAFE_Z13:
                return "GO_SAFE_Z13";
            case GO_SAFE_Z23:
                return "GO_SAFE_Z23";
            case GO_UP_CLENCHED_Z3:
                return "GO_UP_CLENCHED_Z3";
            case GO_LOW_CLENCHED_Z3:
                return "GO_LOW_CLENCHED_Z3";
            case GO_DROP_Z3:
                return "GO_DROP_Z3";
            case GO_UP_Z3:
                return "GO_UP_Z3";
            case __SAME__:
                return "__SAME__";
            default:
                return "";
        }
    }

    bool go_step(Input input) {
        bool is_done_all = input.is_done_m1 &&
                           input.is_done_m2 &&
                           input.is_done_m3 &&
                           input.is_done_m4 &&
                           input.is_done_m5 &&
                           input.is_done_m6;

        State next_state = __SAME__;

        // Compute next state
        if (0) {
        } else if (state == BEGIN) {
            next_state = GO_WAIT;

        } else if (state == GO_WAIT && is_done_all) {
            next_state = WAIT;

        } else if (state == WAIT && input.cargo_is_processed_Z1 /* && Z1_done != 3 */) {
            next_state = GO_SAFE_Z1;

        } else if (state == GO_SAFE_Z1 && is_done_all) {
            next_state = GO_UP_Z1;

        } else if (state == GO_UP_Z1 && is_done_all) {
            next_state = GO_LOW_Z1;

        } else if (state == GO_LOW_Z1 && is_done_all) {
            next_state = GO_PICKUP_Z1;

        } else if (state == GO_PICKUP_Z1 && is_done_all) {
            next_state = GO_UP_CLENCHED_Z1;

        } else if (state == GO_UP_CLENCHED_Z1 && is_done_all) {
            next_state = GO_SAFE_CLENCHED_Z1;

        } else if (state == WAIT && input.cargo_is_processed_Z2) {
            next_state = GO_SAFE_Z2;

        } else if (state == GO_SAFE_Z2 && is_done_all) {
            next_state = GO_UP_Z2;

        } else if (state == GO_UP_Z2 && is_done_all) {
            next_state = GO_LOW_Z2;

        } else if (state == GO_LOW_Z2 && is_done_all) {
            next_state = GO_PICKUP_Z2;

        } else if (state == GO_PICKUP_Z2 && is_done_all) {
            next_state = GO_UP_CLENCHED_Z2;

        } else if (state == GO_UP_CLENCHED_Z2 && is_done_all) {
            next_state = GO_SAFE_CLENCHED_Z2;

        } else if (state == GO_SAFE_CLENCHED_Z1 && is_done_all) {
            next_state = GO_SAFE_Z13;

        } else if (state == GO_SAFE_CLENCHED_Z2 && is_done_all) {
            next_state = GO_SAFE_Z23;

        } else if ((state == GO_SAFE_Z13 || state == GO_SAFE_Z23) && is_done_all) {
            next_state = GO_UP_CLENCHED_Z3;

        } else if (state == GO_UP_CLENCHED_Z3 && is_done_all) {
            next_state = GO_LOW_CLENCHED_Z3;

        } else if (state == GO_LOW_CLENCHED_Z3 && is_done_all) {
            next_state = GO_DROP_Z3;

        } else if (state == GO_DROP_Z3 && is_done_all) {
            next_state = GO_UP_Z3;

        } else if (state == GO_UP_Z3 && is_done_all) {
            next_state = GO_WAIT;
        }

        // Compute outputs
        if (0) {
        } else if (next_state == GO_WAIT) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A140;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A0;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == WAIT) {
            // do nothing
        } else if (next_state == GO_SAFE_Z1) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z1) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_LOW_Z1) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A90;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_PICKUP_Z1) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A90;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_CLENCHED_Z1) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_CLENCHED_Z1) {
            out.go_base = AngleBase::A180;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_Z2) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z2) {
            out.go_base = AngleBase::A60;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_LOW_Z2) {
            out.go_base = AngleBase::A60;
            out.go_shoulder = AngleShoulder::A90;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_PICKUP_Z2) {
            out.go_base = AngleBase::A60;
            out.go_shoulder = AngleShoulder::A90;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_CLENCHED_Z2) {
            out.go_base = AngleBase::A60;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_CLENCHED_Z2) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_Z13) {
            out.go_base = AngleBase::A180;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_Z23) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_CLENCHED_Z3) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_LOW_CLENCHED_Z3) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A100;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_DROP_Z3) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A100;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z3) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else {
            // do nothing
        }

        // Update the state
        if (next_state != __SAME__) {
            state = next_state;
        }

        // return is_active
        return (next_state != __SAME__);
    }
};

#endif
