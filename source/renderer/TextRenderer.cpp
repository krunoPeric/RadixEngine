#include <radix/renderer/TextRenderer.hpp>

#include <radix/renderer/Renderer.hpp>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include <SDL2/SDL_timer.h>

#include <radix/model/MeshLoader.hpp>
#include <radix/texture/TextureLoader.hpp>
#include <radix/text/FontLoader.hpp>
#include <radix/shader/ShaderLoader.hpp>

#include <radix/World.hpp>
#include <radix/texture/Texture.hpp>
#include <radix/text/Font.hpp>
#include <radix/text/Glyph.hpp>
#include <radix/material/MaterialLoader.hpp>

#include <radix/Camera.hpp>
#include <radix/Viewport.hpp>

#include <epoxy/gl.h>

#include <radix/core/math/Math.hpp>
#include <radix/core/math/Matrix3f.hpp>
#include <radix/core/math/Vector2f.hpp>
#include <radix/core/math/Vector3f.hpp>
#include <radix/core/math/Vector4f.hpp>

#include <radix/component/Transform.hpp>
#include <radix/component/MeshDrawable.hpp>
#include <radix/component/ViewFrame.hpp>
#include <radix/env/Environment.hpp>
#include <malloc.h>

namespace radix {

TextRenderer::TextRenderer(World &w, Renderer &ren) :
  world(w),
  renderer(ren) {
  renderContext = std::make_unique<RenderContext>(ren);
}

int TextRenderer::getTextWidth(std::string text, Font font) {
  return font.getStringLength(text);
}

void TextRenderer::renderText(RenderContext &rc, Text text) {
  // FIXME This should be determined by the currently set font
  const Material &mat = MaterialLoader::fromTexture("Pacaya.png");
  Shader &shader = ShaderLoader::getShader("text.frag");
  shader.bind();
  Vector4f color = text.color;
  glUniform4f(shader.uni("color"), color.x, color.y, color.z, color.w);
  float* uniform = (float*)malloc(sizeof(Vector4f));
  glGetUniformfv(shader.handle, shader.uni("color"), uniform);
  free(uniform);
  Vector2f position(text.position.x, text.position.y);
  Matrix4f mtx;

  const char *array = text.content.c_str();
  for (unsigned int i = 0; i < text.content.length(); i++) {
    char c = array[i];

    Font font = FontLoader::getFont(text.font);
    font.size = text.size;
    const Glyph &letter = font.getGlyph(c);
    const Mesh &mesh = letter.mesh;

    mtx.setIdentity();
    mtx.translate(Vector3f(position.x + letter.xOffset * font.size,
                  position.y + letter.yOffset * font.size,
                  text.position.z));

    mtx.scale(Vector3f(letter.width * font.size,
                      letter.height * font.size, 1));

    renderer.renderMesh(rc, shader, mtx, mesh, mat);
    position.x += letter.advance * font.size;
  }
  shader.release();
}
} /* namespace radix */
