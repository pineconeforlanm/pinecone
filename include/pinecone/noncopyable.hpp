//
// Created by 28192 on 2024/9/14.
//

#ifndef PINECONE_NONCOPYABLE_H
#define PINECONE_NONCOPYABLE_H

namespace pinecone {
class Noncopyable {
 public:
  Noncopyable(const Noncopyable &) = delete;

  void operator=(const Noncopyable &) = delete;

 protected:
  Noncopyable() = default;

  ~Noncopyable() = default;
};
}  // namespace pinecone

#endif  // PINECONE_NONCOPYABLE_H
