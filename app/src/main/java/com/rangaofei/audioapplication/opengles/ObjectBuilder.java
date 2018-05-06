package com.rangaofei.audioapplication.opengles;

import java.util.ArrayList;
import java.util.List;

import static android.opengl.GLES20.GL_TRIANGLE_FAN;
import static android.opengl.GLES20.GL_TRIANGLE_STRIP;
import static android.opengl.GLES20.glDrawArrays;

/**
 * Created by rangaofei on 2018/4/27.
 */

public class ObjectBuilder {
    private static final int FLOATS_PER_VERTEX = 3;
    private final float[] vertexData;
    private int offset = 0;

    private final List<DrawCommand> drawList = new ArrayList<>();


    static interface DrawCommand {
        void draw();
    }

    static class GenerateData {
        final float[] vertexData;
        final List<DrawCommand> drawList;

        public GenerateData(float[] vertexData, List<DrawCommand> drawList) {
            this.vertexData = vertexData;
            this.drawList = drawList;
        }
    }

    private ObjectBuilder(int sizeInVertices) {
        vertexData = new float[sizeInVertices * FLOATS_PER_VERTEX];
    }

    public static int sizeOfCircleInVertices(int numPoints) {
        return 1 + (numPoints + 1);
    }

    public static int sizeOfOpenCylinderInVertices(int numPoints) {
        return (numPoints + 1) * 2;
    }

    public static GenerateData createPuck(Geometry.Cylinder puck, int numPoints) {
        int size = sizeOfCircleInVertices(numPoints) + sizeOfOpenCylinderInVertices(numPoints);
        ObjectBuilder builder = new ObjectBuilder(size);

        Geometry.Circle puckTop = new Geometry.Circle(
                puck.center.translateY(puck.height / 2f),
                puck.radius
        );
        builder.appendCircle(puckTop, numPoints);
        builder.appendOpenCylinder(puck, numPoints);
        return builder.build();
    }

    private void appendOpenCylinder(Geometry.Cylinder puck, int numPoints) {
        final int startVertex = offset / FLOATS_PER_VERTEX;
        final int numVertices = sizeOfOpenCylinderInVertices(numPoints);
        final float yStart = puck.center.y - (puck.height / 2f);
        final float yEnd = puck.center.y + (puck.height / 2f);

        for (int i = 0; i <= numPoints; i++) {
            float angleInRadians = (float) i / numPoints * (float) Math.PI * 2f;
            float xPosition = puck.center.x + puck.radius * (float) Math.cos(angleInRadians);
            float zPosition = puck.center.z + puck.radius * (float) Math.sin(angleInRadians);
            vertexData[offset++] = xPosition;
            vertexData[offset++] = yStart;
            vertexData[offset++] = zPosition;
            vertexData[offset++] = xPosition;
            vertexData[offset++] = yEnd;
            vertexData[offset++] = zPosition;
        }
        drawList.add(new DrawCommand() {
            @Override
            public void draw() {
                glDrawArrays(GL_TRIANGLE_STRIP, startVertex, numVertices);
            }
        });
    }


    private void appendCircle(Geometry.Circle puckTop, final int numPoints) {
        final int startVertex = offset/FLOATS_PER_VERTEX;
        final int numVertices = sizeOfCircleInVertices(numPoints);
        vertexData[offset++] = puckTop.center.x;
        vertexData[offset++] = puckTop.center.y;
        vertexData[offset++] = puckTop.center.z;

        for (int i = 0; i <= numPoints; i++) {
            float angleInRadians = (float) i / numPoints * (float) Math.PI * 2f;
            vertexData[offset++] = puckTop.center.x + puckTop.radius * (float) Math.cos(angleInRadians);
            vertexData[offset++] = puckTop.center.y;
            vertexData[offset++] = puckTop.center.z + puckTop.radius * (float) Math.sin(angleInRadians);
        }
        drawList.add(new DrawCommand() {
            @Override
            public void draw() {
                glDrawArrays(GL_TRIANGLE_FAN, startVertex, numVertices);
            }
        });
    }





    private GenerateData build() {
        return new GenerateData(vertexData, drawList);
    }

    static GenerateData createMallet(Geometry.Point center, float radius, float height, int numPoints) {
        int size = sizeOfCircleInVertices(numPoints) * 2 + sizeOfOpenCylinderInVertices(numPoints) * 2;
        ObjectBuilder builder = new ObjectBuilder(size);
        float baseHeight = height * 0.25f;

        Geometry.Circle baseCircle = new Geometry.Circle(center.translateY(-baseHeight / 2f),
                radius);
        Geometry.Cylinder baseCylinder = new Geometry.Cylinder(
                baseCircle.center.translateY(-baseHeight / 2f),
                radius, baseHeight);

        builder.appendCircle(baseCircle, numPoints);
        builder.appendOpenCylinder(baseCylinder, numPoints);

        float handleHeight = height * 0.75f;
        float handleRadius = radius / 3f;

        Geometry.Circle handleCircle = new Geometry.Circle(
                center.translateY(height * 0.5f), handleRadius
        );

        Geometry.Cylinder handleCylinder = new Geometry.Cylinder(
                handleCircle.center.translateY(-handleHeight / 2f),
                handleRadius, handleHeight
        );
        builder.appendCircle(handleCircle, numPoints);
        builder.appendOpenCylinder(handleCylinder, numPoints);
        return builder.build();
    }
}
