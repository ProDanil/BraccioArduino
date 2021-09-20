#ifndef Controller1_hpp
#define Controller1_hpp

struct ControllerX1 {
    enum struct AngleBase {
        A0 = 0,
        A60 = 60,
        A90 = 90,
        A120 = 120,
        A180 = 180,
    };
    enum struct AngleShoulder {
        A65 = 65,
        A80 = 80,
        A90 = 90,
        A115 = 115,
        A140 = 140,
    };
    enum struct AngleElbow {
        A0 = 0,
        A180 = 180,
    };
    enum struct AngleWristVer {
        A10 = 10,
        A15 = 15,
        A160 = 160,
        A165 = 165,
        A170 = 170,
    };
    enum struct AngleWristRot {
        A0 = 0,
        A90 = 90,
    };
    enum struct AngleGripper {
        A15 = 15,
        A73 = 73,
    };

    enum State {
        BEGIN,
        GO_WAIT,
        WAIT,
        GO_SAFE_Z01,
        GO_UP_Z01,
        GO_LOW_Z01,
        GO_PICKUP_Z01,
        GO_UP_PICKUP_Z01,
        GO_SAFE_PICKUP_Z01,
        GO_SAFE_Z02,
        GO_UP_Z02,
        GO_LOW_Z02,
        GO_PICKUP_Z02,
        GO_UP_PICKUP_Z02,
        GO_SAFE_PICKUP_Z02,
        GO_SAFE_Z1,
        GO_UP_PICKUP_Z1,
        GO_LOW_PICKUP_Z1,
        GO_DROP_Z1,
        GO_UP_Z1,
        GO_SAFE_Z2,
        GO_UP_PICKUP_Z2,
        GO_LOW_PICKUP_Z2,
        GO_DROP_Z2,
        GO_UP_Z2,
        __SAME__
    } state = BEGIN;

    State next_state = __SAME__;

    struct Out {
        /* TODO: initial values */
        AngleBase go_base;
        AngleShoulder go_shoulder;
        AngleElbow go_elbow;
        AngleWristVer go_wrist_ver;
        AngleWristRot go_wrist_rot;
        AngleGripper go_gripper;
    } out;

    Out go_step(bool want_cargo_on_Z1, bool want_cargo_on_Z2,
                bool cargo_on_Z1, bool cargo_on_Z2,
                bool is_done_m1, bool is_done_m2,
                bool is_done_m3, bool is_done_m4,
                bool is_done_m5, bool is_done_m6) {
        bool is_done_all = is_done_m1 && is_done_m2 && is_done_m3 && is_done_m4 && is_done_m5 && is_done_m6;

        if (0) {
        } else if (state == BEGIN) {
            next_state = GO_WAIT;

        } else if (state == GO_WAIT && is_done_all) {
            next_state = WAIT;

        } else if (state == WAIT && want_cargo_on_Z1) {
            next_state = GO_SAFE_Z01;

        } else if (state == GO_SAFE_Z01 && is_done_all && want_cargo_on_Z1) {
            next_state = GO_UP_Z01;

        } else if (state == GO_UP_Z01 && is_done_all && want_cargo_on_Z1) {
            next_state = GO_LOW_Z01;

        } else if (state == GO_LOW_Z01 && is_done_all && want_cargo_on_Z1) {
            next_state = GO_PICKUP_Z01;

        } else if (state == GO_PICKUP_Z01 && is_done_all) {
            next_state = GO_UP_PICKUP_Z01;

        } else if (state == GO_UP_PICKUP_Z01 && is_done_all) {
            next_state = GO_SAFE_PICKUP_Z01;

        } else if (state == WAIT && want_cargo_on_Z2) {
            next_state = GO_SAFE_Z02;

        } else if (state == GO_SAFE_Z02 && is_done_all && want_cargo_on_Z2) {
            next_state = GO_UP_Z02;

        } else if (state == GO_UP_Z02 && is_done_all && want_cargo_on_Z2) {
            next_state = GO_LOW_Z02;

        } else if (state == GO_LOW_Z02 && is_done_all && want_cargo_on_Z2) {
            next_state = GO_PICKUP_Z02;

        } else if (state == GO_PICKUP_Z02 && is_done_all) {
            next_state = GO_UP_PICKUP_Z02;

        } else if (state == GO_UP_PICKUP_Z02 && is_done_all) {
            next_state = GO_SAFE_PICKUP_Z02;

        } else if (state == GO_SAFE_PICKUP_Z01 && !cargo_on_Z1 && is_done_all) {
            next_state = GO_SAFE_Z1;

        } else if (state == GO_SAFE_Z1 && is_done_all) {
            next_state = GO_UP_PICKUP_Z1;

        } else if (state == GO_UP_PICKUP_Z1 && is_done_all) {
            next_state = GO_LOW_PICKUP_Z1;

        } else if (state == GO_LOW_PICKUP_Z1 && is_done_all) {
            next_state = GO_DROP_Z1;

        } else if (state == GO_DROP_Z1 && is_done_all) {
            next_state = GO_UP_Z1;

        } else if ((state == GO_UP_Z1 || state == GO_SAFE_Z01 || state == GO_UP_Z01 || state == GO_LOW_Z01) && is_done_all) {
            next_state = GO_WAIT;

        } else if (state == GO_SAFE_PICKUP_Z02 && !cargo_on_Z2 && is_done_all) {
            next_state = GO_SAFE_Z2;

        } else if (state == GO_SAFE_Z2 && is_done_all) {
            next_state = GO_UP_PICKUP_Z2;

        } else if (state == GO_UP_PICKUP_Z2 && is_done_all) {
            next_state = GO_LOW_PICKUP_Z2;

        } else if (state == GO_LOW_PICKUP_Z2 && is_done_all) {
            next_state = GO_DROP_Z2;

        } else if (state == GO_DROP_Z2 && is_done_all) {
            next_state = GO_UP_Z2;

        } else if ((state == GO_UP_Z2 || state == GO_SAFE_Z02 || state == GO_UP_Z02 || state == GO_LOW_Z02) && is_done_all) {
            next_state = GO_WAIT;

        }

        // ==========================================

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
        } else if (next_state == GO_SAFE_Z01) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A160;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z01) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A160;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_LOW_Z01) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A80;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A170;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_PICKUP_Z01) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A80;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A170;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_PICKUP_Z01) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_PICKUP_Z01) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_Z02) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A160;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z02) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A160;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_LOW_Z02) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A80;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A170;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_PICKUP_Z02) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A80;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A170;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_PICKUP_Z02) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_PICKUP_Z02) {
            out.go_base = AngleBase::A180;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_SAFE_Z1) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_PICKUP_Z1) {
            out.go_base = AngleBase::A60;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_LOW_PICKUP_Z1) {
            out.go_base = AngleBase::A60;
            out.go_shoulder = AngleShoulder::A90;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_DROP_Z1) {
            out.go_base = AngleBase::A60;
            out.go_shoulder = AngleShoulder::A90;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z1) {
            out.go_base = AngleBase::A60;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_SAFE_Z2) {
            out.go_base = AngleBase::A180;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_PICKUP_Z2) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_LOW_PICKUP_Z2) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A90;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_DROP_Z2) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A90;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z2) {
            out.go_base = AngleBase::A120;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else {
            // do nothing
        }

        state = next_state;
        return out;
    }
};

#endif
