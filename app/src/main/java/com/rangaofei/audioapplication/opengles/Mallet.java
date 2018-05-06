package com.rangaofei.audioapplication.opengles;

import java.util.List;

/**
 * Created by rangaofei on 2018/4/27.
 */

public class Mallet {
    private static final int POSITION_COMPONENT_COUNT = 3;

    public final float radius;
    public final float height;

    private final VertexArray vertexArray;
    private final List<ObjectBuilder.DrawCommand> drawList;

    public Mallet(float radius, float height, int numPointsAroundMallet) {
        ObjectBuilder.GenerateData generateData = ObjectBuilder.createMallet(
                new Geometry.Point(0f, 0f, 0f),
                radius, height, numPointsAroundMallet
        );
        this.radius = radius;
        this.height = height;

        vertexArray = new VertexArray(generateData.vertexData);
        drawList = generateData.drawList;
    }


    public void bindData(ColorShaderProgram colorProgram) {
        vertexArray.setVertexAttribPointer(
                0,
                colorProgram.getPositionAttributeLocation(),
                POSITION_COMPONENT_COUNT,
                0
        );
    }

    public void draw() {
        for (ObjectBuilder.DrawCommand drawCommand : drawList) {
            drawCommand.draw();
        }
    }
}
