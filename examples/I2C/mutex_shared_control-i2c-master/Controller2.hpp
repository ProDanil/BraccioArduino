#ifndef Controller2_hpp
#define Controller2_hpp

struct ControllerX2 {
    enum struct AngleBase{
        A0 = 0,
        A60 = 60,
        A90 = 90
    };
    enum struct AngleShoulder{
        A65 = 65,
        A80 = 80,
        A100 = 100,
        A115 = 115,
        A140 = 140
    };
    enum struct AngleElbow{
        A0 = 0,
        A180 = 180
    };
    enum struct AngleWristVer{
        A10 = 10,
        A15 = 15,
        A165 = 165,
        A170 = 170
    };
    enum struct AngleWristRot{
        A0 = 0,
        A90 = 90
    };
    enum struct AngleGripper{
        A15 = 15,
        A73 = 73
    };

    enum State {
        BEGIN,
        GO_WAIT,
        WAIT,
        GO_UP_Z0,
        GO_LOW_Z0,
        GO_PICKUP_Z0,
        GO_UP_PICKUP_Z0,
        WAIT_ACK,
        GO_UP_PICKUP_Z1,
        GO_LOW_PICKUP_Z1,
        GO_DROP_Z1,
        GO_UP_Z1,
        __SAME__
    } state = BEGIN;

    State next_state = __SAME__;

    struct Input {
        bool want_cargo_on_out;
        bool is_acquired;
        bool is_done_m1; bool is_done_m2;
        bool is_done_m3; bool is_done_m4;
        bool is_done_m5; bool is_done_m6;
    } input;

    struct Out {
        /* TODO: initial values */
        AngleBase go_base;
        AngleShoulder go_shoulder;
        AngleElbow go_elbow;
        AngleWristVer go_wrist_ver;
        AngleWristRot go_wrist_rot;
        AngleGripper go_gripper;
        bool active;
        bool want_to_release;
        bool want_to_acquire;
    } out;

    Out go_step(Input input) {
        bool is_done_all = input.is_done_m1 && input.is_done_m2 && input.is_done_m3 && input.is_done_m4 && input.is_done_m5 && input.is_done_m6;

        if (0) {
        } else if (state == BEGIN) {
            next_state = GO_WAIT;

        } else if (state == GO_WAIT && is_done_all) {
            next_state = WAIT;

        } else if (state == WAIT && input.want_cargo_on_out) {
            next_state = GO_UP_Z0;

        } else if (state == GO_UP_Z0 && is_done_all && input.want_cargo_on_out) {
            next_state = GO_LOW_Z0;

        } else if (state == GO_LOW_Z0 && is_done_all && input.want_cargo_on_out) {
            next_state = GO_PICKUP_Z0;

        } else if (state == GO_PICKUP_Z0 && is_done_all) {
            next_state = GO_UP_PICKUP_Z0;

        } else if (state == GO_UP_PICKUP_Z0 && is_done_all) {
            next_state = WAIT_ACK;

        } else if (state == WAIT_ACK && input.is_acquired) {
            next_state = GO_UP_PICKUP_Z1;

        } else if (state == GO_UP_PICKUP_Z1 && is_done_all) {
            next_state = GO_LOW_PICKUP_Z1;

        } else if (state == GO_LOW_PICKUP_Z1 && is_done_all) {
            next_state = GO_DROP_Z1;

        } else if (state == GO_DROP_Z1 && is_done_all) {
            next_state = GO_UP_Z1;

        } else if ((state == GO_UP_Z1 || state == GO_UP_Z0 || state == GO_LOW_Z0) && is_done_all) {
            next_state = GO_WAIT;
        }

        // ==========================================

        if (0) {
        } else if (next_state == GO_WAIT) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A140;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A0;
            out.go_gripper = AngleGripper::A15;
            out.want_to_release = false;
        } else if (next_state == WAIT) {
            out.want_to_release = false;
        } else if (next_state == GO_UP_Z0) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_LOW_Z0) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A100;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_PICKUP_Z0) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A100;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_UP_PICKUP_Z0) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
            out.want_to_acquire = true;
        } else if (next_state == WAIT_ACK) {
            //do nothing
        } else if (next_state == GO_UP_PICKUP_Z1) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
            out.want_to_acquire = false;
        } else if (next_state == GO_LOW_PICKUP_Z1) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A80;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A170;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == GO_DROP_Z1) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A80;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A170;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP_Z1) {
            out.go_base = AngleBase::A90;
            out.go_shoulder = AngleShoulder::A65;
            out.go_elbow = AngleElbow::A180;
            out.go_wrist_ver = AngleWristVer::A165;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
            out.want_to_release = true;
        } else {
            // do nothing
        }

        if (state == next_state){
            out.active = false;
        } else {
            out.active = true;
        }
        state = next_state;
        return out;
    }
};

#endif
