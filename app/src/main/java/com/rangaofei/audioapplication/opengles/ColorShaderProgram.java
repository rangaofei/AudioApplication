package com.rangaofei.audioapplication.opengles;

import android.content.Context;

import com.rangaofei.audioapplication.R;

import static android.opengl.GLES20.glGetAttribLocation;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glUniform4f;
import static android.opengl.GLES20.glUniformMatrix4fv;

/**
 * Created by rangaofei on 2018/4/27.
 */

class ColorShaderProgram extends ShaderProgram {

    private final int uMatrixLocation;

    private final int aPositionLocation;

    private final int uColorLocation;

    protected ColorShaderProgram(Context context) {
        super(context, R.raw.simple_vertex_shader, R.raw.simple_fragment_shader);
        uMatrixLocation = glGetUniformLocation(program, U_MATRIX);
        aPositionLocation = glGetAttribLocation(program, A_POSTION);
        uColorLocation = glGetUniformLocation(program, U_COLOR);
    }

    public void setUniforms(float[] matrix, float r, float g, float b) {
        glUniformMatrix4fv(uMatrixLocation, 1, false, matrix, 0);
        glUniform4f(uColorLocation, r, g, b, 1f);
    }

    public int getPositionAttributeLocation() {
        return aPositionLocation;
    }

}
