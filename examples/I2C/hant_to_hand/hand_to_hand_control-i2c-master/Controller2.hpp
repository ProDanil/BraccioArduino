#ifndef Controller2_hpp
#define Controller2_hpp

struct ControllerX2 {
    enum struct AngleBase {
        A0 = 0,
        A180 = 180
    };
    enum struct AngleShoulder {
        A100 = 100,
        A115 = 115,
        A120 = 120,
        A140 = 140
    };
    enum struct AngleElbow {
        A0 = 0,
        A30 = 30
    };
    enum struct AngleWristVer {
        A10 = 10,
        A15 = 15,
        A25 = 25,
        A30 = 30,
        A50 = 50
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
        GO_PREPOS, 
        GO_UP, 
        GO_PICKUP,
        WAIT_SAFE, 
        GO_SAFE_OUT,
        GO_UP_OUT_PICKUP, 
        GO_LOW_OUT_PICKUP, 
        GO_DROP,
        __SAME__
    } state = BEGIN;

    struct Input {
        bool X1_ready;
        bool is_done_m1;
        bool is_done_m2;
        bool is_done_m3;
        bool is_done_m4;
        bool is_done_m5;
        bool is_done_m6;

        bool operator==(const Input& other) const {
            return X1_ready == other.X1_ready &&
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
        bool ack;
        String st;
    } out;

    String state2string(State s) {
        switch (s) {
            case BEGIN:
                return "BEGIN";
            case GO_WAIT:
                return "GO_WAIT";
            case WAIT:
                return "WAIT";
            case GO_PREPOS:
                return "GO_PREPOS";
            case GO_UP:
                return "GO_UP";
            case GO_PICKUP:
                return "GO_PICKUP";
            case WAIT_SAFE:
                return "WAIT_SAFE";   
            case GO_SAFE_OUT:
                return "GO_SAFE_OUT";
            case GO_UP_OUT_PICKUP:
                return "GO_UP_OUT_PICKUP";
            case GO_LOW_OUT_PICKUP:
                return "GO_LOW_OUT_PICKUP";
            case GO_DROP:
                return "GO_DROP";
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

        } else if (state == WAIT && input.X1_ready) {
            next_state = GO_PREPOS;

        } else if (state == GO_PREPOS && is_done_all) {
            next_state = GO_UP;

        } else if (state == GO_UP && is_done_all) {
            next_state = GO_PICKUP;

        } else if (state == GO_PICKUP && is_done_all) {
            next_state = WAIT_SAFE;

        } else if (state == WAIT_SAFE && input.X1_ready) {
            next_state = GO_SAFE_OUT;

        } else if (state == GO_SAFE_OUT && is_done_all) {
            next_state = GO_UP_OUT_PICKUP;

        } else if (state == GO_UP_OUT_PICKUP && is_done_all) {
            next_state = GO_LOW_OUT_PICKUP;

        } else if (state == GO_LOW_OUT_PICKUP && is_done_all) {
            next_state = GO_DROP;

        } else if (state == GO_DROP && is_done_all) {
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

        } else if (next_state == GO_PREPOS) {
            out.go_base = AngleBase::A180;
            out.go_shoulder = AngleShoulder::A120;
            out.go_elbow = AngleElbow::A30;
            out.go_wrist_ver = AngleWristVer::A50;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_UP) {
            out.go_base = AngleBase::A180;
            out.go_shoulder = AngleShoulder::A120;
            out.go_elbow = AngleElbow::A30;
            out.go_wrist_ver = AngleWristVer::A25;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A15;
        } else if (next_state == GO_PICKUP) {
            out.go_base = AngleBase::A180;
            out.go_shoulder = AngleShoulder::A120;
            out.go_elbow = AngleElbow::A30;
            out.go_wrist_ver = AngleWristVer::A25;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
        } else if (next_state == WAIT_SAFE) {
            out.ack = true;
        } else if (next_state == GO_SAFE_OUT) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A120;
            out.go_elbow = AngleElbow::A30;
            out.go_wrist_ver = AngleWristVer::A30;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
            out.ack = false;
        } else if (next_state == GO_UP_OUT_PICKUP) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A115;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A15;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
            out.ack = true;
        } else if (next_state == GO_LOW_OUT_PICKUP) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A100;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A90;
            out.go_gripper = AngleGripper::A73;
            out.ack = false;
        } else if (next_state == GO_DROP) {
            out.go_base = AngleBase::A0;
            out.go_shoulder = AngleShoulder::A140;
            out.go_elbow = AngleElbow::A0;
            out.go_wrist_ver = AngleWristVer::A10;
            out.go_wrist_rot = AngleWristRot::A0;
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
