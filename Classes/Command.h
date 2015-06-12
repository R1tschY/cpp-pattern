#ifndef CLASSES_COMMAND_H_
#define CLASSES_COMMAND_H_

class Command {
public:
  virtual ~Command();

  virtual void execute() = 0;

  // optional
  virtual void undo() = 0;
};

#endif /* CLASSES_COMMAND_H_ */
