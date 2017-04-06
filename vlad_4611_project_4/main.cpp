#include "engine.hpp"
#include "camera.hpp"
#include "character.hpp"
#include "config.hpp"
#include "draw.hpp"
#include "spline.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <iomanip>
using namespace std;
using glm::vec3;
using glm::vec4;

class SplineWalker: public Engine {
public:
	SDL_Window *window;
    OrbitCamera *camera;

    Character *character;
    Spline3 *path;
    float time; // time along the path

    SplineWalker() {
        window = createWindow("Walk the Spline", 640, 360);
        camera = new OrbitCamera(5, 0, 0, Perspective(30, 16/9., 0.1, 20));
        character = new Character(Config::asfFile, Config::amcFile,
                                  Config::basePosition, Config::baseVelocity);
        if (!character->hasSkeleton()) {
            errorMessage("Failed to load file " + Config::asfFile);
            exit(EXIT_FAILURE);
        }
        if (!character->hasAnimation()) {
            errorMessage("Failed to load file " + Config::amcFile);
            exit(EXIT_FAILURE);
        }
        path = new Spline3;


		int choice = 4;
		if (choice == 1) {
        //Constant-velocity line
        path->points.push_back(SplinePoint3(0, vec3(0,0,0), vec3(1.5,0,0)));
        path->points.push_back(SplinePoint3(10, vec3(15,0,0), vec3(1.5,0,0)));
		} else if (choice == 2) {
        // Ease-in ease-out line
        path->points.push_back(SplinePoint3(0, vec3(0,0,0), vec3(0,0,0)));
        path->points.push_back(SplinePoint3(10, vec3(15,0,0), vec3(0,0,0)));
		} else if (choice == 3) {
        // Approximately circular path
        path->points.push_back(SplinePoint3(0, vec3(5,0,0), vec3(0,0,1.5)));
        path->points.push_back(SplinePoint3(5, vec3(0,0,5), vec3(-1.5,0,0)));
        path->points.push_back(SplinePoint3(10, vec3(-5,0,0), vec3(0,0,-1.5)));
        path->points.push_back(SplinePoint3(15, vec3(0,0,-5), vec3(1.5,0,0)));
        path->points.push_back(SplinePoint3(20, vec3(5,0,0), vec3(0,0,1.5)));
		} else if (choice == 4) {
        //// Figure-eight path
        path->points.push_back(SplinePoint3(0, vec3(5,0,0), vec3(0,0,1)));
        path->points.push_back(SplinePoint3(5, vec3(0,0,0), vec3(-1,0,-1)));
        path->points.push_back(SplinePoint3(10, vec3(-5,0,0), vec3(0,0,1)));
        path->points.push_back(SplinePoint3(15, vec3(0,0,0), vec3(1,0,-1)));
        path->points.push_back(SplinePoint3(20, vec3(5,0,0), vec3(0,0,1)));
		}

        time = 0;
    }

    ~SplineWalker() {
        SDL_DestroyWindow(window);
    }

    void run() {
        float fps = 60, dt = 1/fps;
        while (!shouldQuit()) {
            handleInput();
            advanceState(dt);
            drawGraphics();
            waitForNextFrame(dt);
        }
    }

    void advanceState(float dt) {
        time += dt;
        if (time > path->maxTime())
            time = path->minTime();

        // TODO: Modify this to control the speed of the character's
        // walk cycle animation.


		
		if (bool enableSpeedAdjustment = true) {
			float baseSpeed		= glm::length(Config::baseVelocity);
			float currentSpeed	= glm::length(path->getDerivative(time)); //length of this derivative is speed.
			character->advance(dt * (currentSpeed / baseSpeed)*.25);
		} else {
			character->advance(dt); 
		}

        vec3 p = path->getValue(time);
        vec3 c = camera->getCenter();
        camera->setCenter(glm::mix(c, vec3(p.x, 0.8, p.z), 10*dt));
    }

    void setAmbientLight(vec3 color) {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &color[0]);
    }

    void addLight(GLenum light, vec4 position, vec3 color) {
        glEnable(light);
        glLightfv(light, GL_POSITION, &position[0]);
        glLightfv(light, GL_DIFFUSE, &color[0]);
    }

    void drawGraphics() {
        // Blue sky background
        glClearColor(0.6,0.8,1.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Allow lines to show up on top of filled polygons
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1,1);

        camera->apply();

        //Draw::axes(1);

        // Light from above
        glEnable(GL_LIGHTING);
        setAmbientLight(vec3(0.5,0.5,0.5));
        addLight(GL_LIGHT0, vec4(+1,1,0,0), 0.2f*vec3(1,1,1));
        addLight(GL_LIGHT1, vec4(-1,1,0,0), 0.2f*vec3(1,1,1));
        addLight(GL_LIGHT2, vec4(0,1,-1,0), 0.2f*vec3(1,1,1));
        addLight(GL_LIGHT3, vec4(0,1,+1,0), 0.2f*vec3(1,1,1));

		
		vec3 position		= path->getValue(time);
		vec3 futurePosition = path->getValue(time + 1);

        // Draw floor
        drawFloor(position);

        glColor3f(0.8,0.2,0.2);
        drawSpline(path);

        // TODO: Translate the character to align with the position
        // obtained from the path spline, i.e. path->getValue(time).
        // Also rotate it so its z-axis aligns with the path spline's
        // velocity, path->getDerivative(time).





        glColor3f(1,0.8,0.2);
        glPushMatrix();
			glTranslatef(position.x, position.y, position.z);
			
			vec3  deriv = path->getDerivative(time);
			vec3  b = glm::normalize(deriv);
			vec3  z = vec3(0, 0, 1);
			vec3  rotAxis = glm::normalize(glm::cross(b, z));
			float rotAxisLength = glm::length(rotAxis);
			float angleRad = glm::dot(b, z);
			float angleDeg = (angleRad) * 90;
			glPushMatrix();
				glRotatef(-90 + angleDeg, rotAxis.x, rotAxis.y, rotAxis.z);
				character->draw();
			glPopMatrix();

			//line to future position marked by sphere
			Draw::line(futurePosition - position);
			Draw::sphere(futurePosition - position, .05);

			//direction arrow in direction of derivative
			glPushAttrib(GL_CURRENT_BIT);
				glColor3f(1, 0, 0);
				Draw::line(deriv*.2);
				Draw::sphere(deriv*.2, .05);
			glPopAttrib();
			
        glPopMatrix();


        SDL_GL_SwapWindow(window);
    }

    void drawSpline(Spline3 *spline) {
        glPushAttrib(GL_LINE_BIT);
        glLineWidth(2);
        glBegin(GL_LINE_STRIP);
        glNormal3f(0,1,0);
        for (float t = spline->minTime(); t < spline->maxTime(); t+=0.1) {
            vec3 value = spline->getValue(t);
            glVertex3f(value.x, value.y, value.z);
        }
        glEnd();
        for (int i = 0; i < spline->points.size(); i++) {
            vec3 p = spline->points[i].p;
            glPushMatrix();
            glTranslatef(p.x, p.y, p.z);
            glScalef(0.1,0.1,0.1);
            glTranslatef(-0.5,-0.5,-0.5);
            Draw::unitCube();
            glPopMatrix();
        }
        glPopAttrib();
    }

    void drawFloor(vec3 center) {
        int cx = 2*(int)round(center.x/2), cz = 2*(int)round(center.z/2);
        int n = 10;
        for (int i = -n; i < n; i++) {
            for (int j = -n; j < n; j++) {
                if ((i+j) % 2 == 0)
                    glColor3f(0.8,0.8,0.8);
                else
                    glColor3f(0.6,0.6,0.6);
                glPushMatrix();
                glTranslatef(cx+i, 0, cz+j);
                glTranslatef(0,0,1);
                glRotatef(-90, 1,0,0);
                Draw::unitSquareXY();
                glPopMatrix();
            }
        }
    }

    void onMouseMotion(SDL_MouseMotionEvent &e) {
        camera->onMouseMotion(e);
    }
};

int main(int argc, char **argv) {
    SplineWalker app;
    app.run();
    return EXIT_SUCCESS;
}
