
#pragma once

enum class cam_type {
	free,
	orbit
};

struct camera_free {
	v3 pos, front, up, right, globalUp;
	f32 pitch, yaw, speed, fov;
	u32 lastUpdate;

	m4 view() {
		m4 ret = lookAt(pos, pos + front, up);
		return ret;
	}

	void updateFront() {
		front.x = cosf(RADIANS(pitch)) * cosf(RADIANS(yaw));
		front.y = sinf(RADIANS(pitch));
		front.z = sinf(RADIANS(yaw)) * cosf(RADIANS(pitch));
		front = norm(front);
		right = norm(cross(front, globalUp));
		up = norm(cross(right, front));
	}

	void reset() {
		fov = 60.0f;
		pitch = -45.0f;
		yaw = 225.0f;
		speed = 5.0f;
		pos = v3(5, 5, 5);
		globalUp = v3(0, 1, 0);
		lastUpdate = SDL_GetTicks();
		updateFront();
	}

	void move(i32 dx, i32 dy) {
		const f32 sens = 0.1f;
		yaw += dx * sens;
		pitch -= dy * sens;
		if (yaw > 360.0f) yaw = 0.0f;
		else if (yaw < 0.0f) yaw = 360.0f;
		if (pitch > 89.0f) pitch = 89.0f;
		else if (pitch < -89.0f) pitch = -89.0f;
		updateFront();
	}
};

struct camera_orbit {
	v3 pos, lookingAt, up;
	f32 pitch, yaw, radius, fov;

	m4 view() {
		m4 ret = lookAt(pos, lookingAt, up);
		return ret;
	}

	void setAxis(v3 axis) {

		if(axis == v3(0,0,1)) {
			pitch = 90.0f;
			yaw = 0.0f;
		} else if(axis == v3(0,1,0)) {
			pitch = 0.0f;
			yaw = 0.0f;
		} else if(axis == v3(1,0,0)) {
			pitch = 0.0f;
			yaw = 90.0f;
		}
		updatePos();
	}

	void reset() {
		fov = 60.0f;
		pitch = 45.0f;
		yaw = 45.0f;
		radius = 20.0f;
		lookingAt = v3(0, 0, 0);
		up = v3(0, 1, 0);
		updatePos();
	}

	void updatePos() {
		pos.x = cosf(RADIANS(pitch)) * cosf(RADIANS(yaw));
		pos.y = sinf(RADIANS(pitch));
		pos.z = sinf(RADIANS(yaw)) * cosf(RADIANS(pitch));
		pos = radius * norm(pos) + lookingAt;
	}

	void move(i32 dx, i32 dy) {
		const f32 sens = 0.1f;
		yaw += dx * sens;
		pitch -= dy * sens;
		if (yaw > 360.0f) yaw = 0.0f;
		else if (yaw < 0.0f) yaw = 360.0f;
		if (pitch > 89.9f) pitch = 89.9f;
		else if (pitch < -89.9f) pitch = -89.9f;
		updatePos();
	}
};

